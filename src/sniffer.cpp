#include <spdlog/fmt/fmt.h>
#include <spdlog/fmt/ostr.h>
#include <spdlog/spdlog.h>
#include <tins/tins.h>

#include <chrono>
#include <dminer/parser.hpp>
#include <dminer/sniffer.hpp>
#include <dminer/statistics.hpp>
#include <dminer/utilities.hpp>
#include <filesystem>
#include <optional>
#include <thread>

namespace fs = std::filesystem;

namespace dminer {

Sniffer::Sniffer(const Tins::NetworkInterface &interface,
                 const std::optional<fs::path> &output_file_csv,
                 const std::optional<fs::path> &output_file_pcap,
                 const uint64_t buffer_size,
                 const std::optional<std::string> &meta_round,
                 const uint16_t destination_port)
    : sniffer_{interface.name()}, meta_round_{meta_round}, statistics_{} {
  auto filter = fmt::format(
      "(dst {} or dst {}) and ((icmp and icmp[icmptype] != icmp-echo) or "
      "(icmp6 and icmp6[icmptype] != icmp-echo) or "
      "(src port {}))",
      Utilities::source_ipv4_for(interface).to_string(),
      Utilities::source_ipv6_for(interface).to_string(), destination_port);
  spdlog::info("sniffer_filter={}", filter);

  Tins::SnifferConfiguration config;
  // TODO: What happens the buffer is full?
  // TODO: Log buffer full / dropped packets?
  config.set_buffer_size(buffer_size * 1024);
  config.set_filter(filter);
  config.set_immediate_mode(true);

  // As sniffer does not have set_configuration, we copy...
  sniffer_ = Tins::Sniffer(interface.name(), config);

  if (output_file_csv) {
    output_csv_.open(*output_file_csv);
  }

  if (output_file_pcap) {
    output_pcap_ = Tins::PacketWriter{*output_file_pcap,
                                      Tins::DataLinkType<Tins::EthernetII>()};
  }
}

Sniffer::~Sniffer() {
  // Cleanup resources in case the sniffer was not properly stopped.
  // For example if an exception was raised on the main thread.
  stop();
}

void Sniffer::start() noexcept {
  auto handler = [this](Tins::Packet &packet) {
    auto reply = Parser::parse(packet);

    if (reply) {
      spdlog::trace(reply.value());
      statistics_.icmp_messages_all.insert(reply->src_ip);
      if ((reply->icmp_code == 11) && (reply->src_ip != reply->inner_dst_ip)) {
        statistics_.icmp_messages_path.insert(reply->src_ip);
      }
      output_csv_ << fmt::format("{},{},{}\n", reply->to_csv(),
                                 meta_round_.value_or("1"), "1");
    } else {
      auto data = packet.pdu()->serialize();
      spdlog::warn("invalid_packet_hex={:02x}", fmt::join(data, ""));
      statistics_.received_invalid_count++;
    }

    if (output_pcap_) {
      output_pcap_->write(packet);
    }

    statistics_.received_count++;
    return true;
  };

  thread_ = std::thread([this, handler]() { sniffer_.sniff_loop(handler); });
}

void Sniffer::stop() noexcept {
  if (thread_.joinable()) {
    sniffer_.stop_sniff();
    thread_.join();
  }
}

const Statistics::Sniffer &Sniffer::statistics() const noexcept {
  return statistics_;
}

}  // namespace dminer