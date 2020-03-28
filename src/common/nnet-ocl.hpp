// 2019 Team AobaZero
// This source code is in the public domain.
#pragma once
#if defined(USE_OPENCL_AOBA)
#include "nnet.hpp"
#include "opencl.hpp"
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>
#include <cassert>

class FName;
namespace NNAux {
  using uint = unsigned int;
  constexpr uint nslot = 8U;
}

class ManageSend {
  using uchar = unsigned char;
  using uint  = unsigned int;
  double _time;
  OCL::Memory _mem_work;
  OCL::Event _event;

public:
  void start(const OCL::Queue &queue, uint maxsize_batch) noexcept;
  void push(const OCL::Queue &queue, const void *p, size_t size)
    const noexcept;
  const OCL::Memory &get_work() const noexcept { return _mem_work; }
};

class ManageDecode {
  using uint = unsigned int;
  size_t _one_size_g[3],  _one_size_l[3];
  size_t _zero_size_g[3], _zero_size_l[3];
  size_t _fill_size_g[3], _fill_size_l[3];
  OCL::Kernel _ker_zero_clear, _ker_plane_fill, _ker_set_one;
  uint _nbatch, _nm;

public:
  void start(const OCL::Queue &queue, const OCL::Memory &mem_in,
	     const OCL::Memory &mem_out, uint _index_block,
	     uint maxsize_batch) noexcept;
  void push(const OCL::Queue &queue, uint n_one) noexcept;
};

class ManageRecv {
  using uint = unsigned int;
  OCL::Memory _mem;
  OCL::Event _event[NNAux::nslot];

public:
  void start(const OCL::Queue &queue, size_t size_max, uint nbatch) noexcept;
  void push(const OCL::Queue &queue, void *p, size_t size,
	    uint uslot) noexcept;
  void wait(uint uslot) const noexcept;
  const OCL::Memory &get() const noexcept { return _mem; }
};

struct SgemmParam {
  using uint = unsigned int;
  double time;
  bool do_half, do_wmma;
  uint nl, nlfm, npm, npn, npk, ntm, ntn;
  SgemmParam() noexcept {}
  SgemmParam(bool do_half_, uint nl_, uint nlfm_, uint npm_, uint npn_,
	     uint npk_) noexcept :
  time(0.0), do_half(do_half_), do_wmma(false), nl(nl_), nlfm(nlfm_),
    npm(npm_), npn(npn_), npk(npk_) {}
  SgemmParam(bool do_half_, bool do_wmma_, uint nl_, uint npm_, uint npn_,
	     uint npk_, uint ntm_, uint ntn_) noexcept :
  time(0.0), do_half(do_half_), do_wmma(do_wmma_),
    nl(nl_), npm(npm_), npn(npn_), npk(npk_), ntm(ntm_), ntn(ntn_) {}
  std::string gen_info() const noexcept;
  bool operator<=(const SgemmParam &p) const noexcept {
    if (do_wmma) {
      return (nl <= p.nl && npm <= p.npm && npn <= p.npn && npk <= p.npk
	      && ntm <= p.ntm && ntn <= p.ntn); }
    return (nl <= p.nl && nlfm <= p.nlfm && npm <= p.npm && npn <= p.npn
	    && npk <= p.npk); }
  bool ok() const noexcept {
    if (do_wmma) {
      return (0.0 <= time && 0 < nl && 0 < npm && 0 < npn && 0 < npk
	      && 0 < ntm && 0 < ntn); }
    return (0.0 <= time && 0 < nl && 0 < nlfm && 0 < npm && 0 < npn
	    && 0 < npk); }
};

class ManageComputeMatM {
  using uint = unsigned int;
  OCL::Kernel _ker;
  size_t _size_g[3], _size_l[3];
  uint _nbatch, _nm, _nn, _nk;
public:
  ManageComputeMatM() noexcept {};
  void start(const OCL::Queue &queue, uint nbatch, uint nm0, uint nn0,
	     uint nk0, const SgemmParam &param) noexcept;
  void register_b(const OCL::Memory &mem) const noexcept;
  void register_c(const OCL::Memory &mem) const noexcept;
  void push(const OCL::Queue &queue, const OCL::Memory &mem_a) const noexcept;
  uint get_nm() const noexcept { assert(_ker.ok()); return _nm; }
  uint get_nn() const noexcept { assert(_ker.ok()); return _nn; }
  uint get_nk() const noexcept { assert(_ker.ok()); return _nk; }
};

class ManageSgemm {
  using uint = unsigned int;
  OCL::Memory mem_a, mem_b, mem_c;
  OCL::Kernel _ker_a, _ker_b, _ker_c, _ker_sgemm;
  double _time;
  size_t size_g[3], size_l[3];
  SgemmParam _param;
  uint _nm0, _nn0, _nk0, _nm, _nn, _nk;
  bool _done_load_a, _done_load_b, _do_transa, _do_transb, _do_transc;

public:
  void start(const OCL::Device &dev, const OCL::Queue &queue, bool transa,
	     bool transb, bool do_transa, bool do_transb, bool do_transc,
	     uint nm0, uint nn0, uint nk0, uint offa, uint lda,
	     uint offb, uint ldb, uint offc, uint ldc) noexcept;
  uint get_nm() const noexcept { return _nm; }
  uint get_nn() const noexcept { return _nn; }
  uint get_nk() const noexcept { return _nk; }
  void push(const OCL::Queue &queue) const noexcept;
  void register_a(const OCL::Memory &mem) const noexcept;
  void register_b(const OCL::Memory &mem) const noexcept;
  void register_c(const OCL::Memory &mem) const noexcept;
  void register_a0(const OCL::Memory &mem) const noexcept;
  void register_b0(const OCL::Memory &mem) const noexcept;
  void register_c0(const OCL::Memory &mem) const noexcept;
  void push_load_a(const OCL::Queue &queue) noexcept;
  void push_load_b(const OCL::Queue &queue) noexcept;
  std::string gen_info() const noexcept;
};

class ManageComputeMatV {
  using uint = unsigned int;
  size_t _size_g[3], _size_l[3];
  OCL::Kernel _ker;
  uint _nm;

public:
  ManageComputeMatV() noexcept : _nm(0) {}
  void start(bool store_half, const OCL::Queue &queue,
	     uint nch, uint nb, uint nn, uint nk,
	     const OCL::Memory &mem_matV) noexcept;
  void push(const OCL::Queue &queue, const OCL::Memory &mem_in) const noexcept;
};

class ManageComputeMatA {
  using uint = unsigned int;
  size_t _size_g[3], _size_l[3];
  OCL::Kernel _ker;
  uint _nm;
public:
  ManageComputeMatA() noexcept : _nm(0) {}
  void start(bool load_half, const OCL::Queue &queue, bool flag_join, uint nch,
	     uint nb, uint nm, uint nn, uint nn_out,
	     const OCL::Memory &mem_matM, const OCL::Memory &mem_bypass,
	     const OCL::Memory &mem_output)
    noexcept;
  void push(const OCL::Queue &queue, const OCL::Memory &mean,
	    const OCL::Memory &sd_inv) const noexcept;
};

class ManageComputeMatAV {
  using uint = unsigned int;
  size_t _size_g[3], _size_l[3];
  OCL::Kernel _ker;
public:
  void start(bool do_half, bool do_join, bool do_fork, const OCL::Queue &queue,
	     uint nch, uint nb, uint nm, uint nn, uint nk,
	     const OCL::Memory &mem_matM, const OCL::Memory &mem_matV,
	     const OCL::Memory &mem_bypass) noexcept;
  void push(const OCL::Queue &queue, const OCL::Memory &mean,
	    const OCL::Memory &sd_inv) const noexcept;
};

class ManageTransformValue2 {
  using uint  = unsigned int;
  size_t _size_g[3], _size_l[3];
  OCL::Kernel _ker;
public:
  void start(const OCL::Queue &queue, uint nch, uint nb,
	     const OCL::Memory &mem_in, uint offin, uint nn_out,
	     const OCL::Memory &mem_out) noexcept;
  void push(const OCL::Queue &queue) const noexcept;
};

class ManageResizeBiasReLU {
  using uint = unsigned int;
  size_t _size_g[3], _size_l[3];
  OCL::Kernel _ker;
public:
  void start(const OCL::Queue &queue, uint nm, uint nn, uint ldin, uint ldout,
	     const OCL::Memory &mem_bias, const OCL::Memory &mem_in,
	     const OCL::Memory &mem_out) noexcept;
  void push(const OCL::Queue &queue) const noexcept;
};

class ManageComputeBNReLU {
  using uint = unsigned int;
  size_t _size_g[3], _size_l[3];
  OCL::Kernel _ker;
public:
  void start(const OCL::Queue &queue, uint nch, uint nb, uint nn_in,
	     const OCL::Memory &mean, const OCL::Memory &sd_inv,
	     const OCL::Memory &mem_in, const OCL::Memory &mem_out) noexcept;
  void push(const OCL::Queue &queue) const noexcept;
};

class ManageComputePolicy {
  using uint = unsigned int;
  OCL::Kernel _ker;
public:
  void start(const OCL::Queue &queue, uint nch_in, uint maxsize_batch,
	     const OCL::Memory &mem_nnmove, const OCL::Memory &mem_weight,
	     const OCL::Memory &mem_bias, const OCL::Memory &mem_in,
	     const OCL::Memory &mem_out) noexcept;
  void push(const OCL::Queue &queue, uint ntot_moves, uint offin)
    const noexcept;
};

class NNetOCL {
  using uint   = unsigned int;
  using ushort = unsigned short;
  using uchar  = unsigned char;
  using row_t  = std::unique_ptr<float []>;
  struct CLResWght { OCL::Memory matU, mean, sd_inv; };

  std::mutex _m;
  std::condition_variable _cv;
  uint _pool_slots[NNAux::nslot];
  uint _pool_size;

  ManageSend _mng_send;
  ManageDecode _mng_decode;
  ManageRecv _mng_recv;
  ManageComputeMatM _mng_compute_matM_input, _mng_compute_matM;
  ManageSgemm _mng_head1, _mng_value2, _mng_value3;
  ManageComputeMatV _mng_compute_matV_input;
  ManageComputeMatA _mng_compute_matA_join;
  ManageComputeMatAV _mng_compute_matAV_input;
  ManageComputeMatAV _mng_compute_matAV, _mng_compute_matAV_join;
  ManageComputeBNReLU _mng_compute_BNReLU;
  ManageTransformValue2 _mng_transform_value2;
  ManageResizeBiasReLU _mng_resize_bias_ReLU_value3;
  ManageComputePolicy _mng_compute_policy;
  OCL::Device _cl_dev;
  OCL::Queue _queue;
  OCL::Kernel _ker_zero_clear;
  OCL::Memory _cl_bypass, _cl_output, _cl_result, _cl_matM, _cl_matV;
  OCL::Memory _cl_head1_wght, _cl_head1_mean, _cl_head1_sd_inv;
  OCL::Memory _cl_policy2_wght, _cl_policy2_bias, _cl_value2_bias;
  OCL::Memory _cl_value2_wght, _cl_value3_wght;
  std::vector<CLResWght> _cl_reswghts;
  std::unique_ptr<uchar []> _ptr_input[NNAux::nslot];
  std::unique_ptr<float []> _ptr_result[NNAux::nslot];
  uint _slots_size_batch[NNAux::nslot];
  std::unique_ptr<uint []> _slots_sizes_nnmove[NNAux::nslot];
  float *_slots_probs[NNAux::nslot];
  float *_slots_values[NNAux::nslot];
  uint _maxsize_batch, _maxsize_out, _resnet_nout, _nres_block;
  uint _head1_nout, _policy1_nout, _value1_nout, _policy2_nin;
  uint _value2_nin, _value2_nout, _value3_nin, _value3_nout, _index_block;
  row_t _value3_bias;
  bool _do_sleep;
  void load(bool use_half, const std::vector<std::pair<uint, row_t>> &wght)
    noexcept;

public:
  std::string reset(uint maxsize_batch,
		    const std::vector<std::pair<uint, row_t>> &wght,
		    int device_id, bool use_half = true, bool flag_out = true,
		    bool do_sleep = false) noexcept;
  void ff(uint size_batch, const float *input, const uint *sizes_nnmove,
	  const ushort *nnmoves, float *probs, float *values) noexcept;
  uint push_ff(uint size_batch, const float *input, const uint *sizes_nnmove,
	       const ushort *nnmoves, float *probs, float *values) noexcept;
  void wait_ff(uint uslot) noexcept;
};
#endif
