// 2019 Team AobaZero
// This source code is in the public domain.
#include "err.hpp"
#include "opencl.hpp"
#include <exception>
#include <iostream>
#include <memory>
#include <tuple>
#include <vector>
#include <cstdlib>
using uint = unsigned int;
using std::unique_ptr;
using std::cout;
using std::cerr;
using std::endl;
using std::get;
using std::make_tuple;
using std::tuple;
using std::vector;
using ErrAux::die;
using namespace OCL;

static void out_platform_info(uint id, const Platform &pf) noexcept {
  cout << "Platform ID: " << id << "\n";
  cout << pf.gen_info(); }

static void out_device_info(uint id, const Device &dev) noexcept {
  cout << "- Device ID: " << id << "\n";
  cout << dev.gen_info(); }

int main() {
  uint platform_id = 0;
  uint device_id   = 0;
  vector<Platform> platforms = gen_platforms();
  for (auto &platform : platforms) {
    out_platform_info(platform_id++, platform);
    
    vector<Device> devices = platform.gen_devices_all();
    for (auto &device : devices) out_device_info(device_id++, device); }

  return 0; }
  
