#pragma once

#include <tins/tins.h>

#include <filesystem>
#include <fstream>
#include <optional>
#include <string>

using std::optional;
using std::string;

namespace fs = std::filesystem;

namespace caracal::Prober {

/// Configuration of the prober.
struct Config {
  uint64_t n_packets = 1;
  uint64_t probing_rate = 100;
  uint64_t sniffer_wait_time = 5;
  string protocol = "udp";
  Tins::NetworkInterface interface =
      Tins::NetworkInterface::default_interface();
  bool allow_sleep_wait = true;
  optional<uint64_t> max_probes;
  optional<fs::path> input_file;
  optional<fs::path> output_file_csv;
  optional<fs::path> output_file_pcap;
  optional<fs::path> prefix_excl_file;
  optional<fs::path> prefix_incl_file;
  optional<int> filter_min_ttl;
  optional<int> filter_max_ttl;
  optional<string> meta_round;

  void set_input_file(const fs::path& p);

  void set_output_file_csv(const fs::path& p);

  void set_output_file_pcap(const fs::path& p);

  void set_probing_rate(int rate);

  void set_protocol(const string& s);

  void set_interface(const string& s);

  void set_sniffer_wait_time(int seconds);

  void set_allow_sleep_wait(bool allow);

  void set_max_probes(uint64_t count);

  void set_n_packets(int count);

  void set_prefix_excl_file(const fs::path& p);

  void set_prefix_incl_file(const fs::path& p);

  void set_filter_min_ttl(int ttl);

  void set_filter_max_ttl(int ttl);

  void set_meta_round(const string& round);
};

std::ostream& operator<<(std::ostream& os, Config const& v);

}  // namespace caracal::Prober