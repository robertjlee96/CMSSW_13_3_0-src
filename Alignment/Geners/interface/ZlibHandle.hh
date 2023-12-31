#ifndef GENERS_ZLIBHANDLE_HH_
#define GENERS_ZLIBHANDLE_HH_

extern "C" {
struct z_stream_s;
}

namespace gs {
  class ZlibInflateHandle {
  public:
    ZlibInflateHandle();
    ~ZlibInflateHandle();

    inline z_stream_s &strm() { return *strm_; }

    ZlibInflateHandle(const ZlibInflateHandle &) = delete;
    ZlibInflateHandle &operator=(const ZlibInflateHandle &) = delete;

  private:
    z_stream_s *strm_;
  };

  class ZlibDeflateHandle {
  public:
    explicit ZlibDeflateHandle(int level);
    ~ZlibDeflateHandle();

    inline z_stream_s &strm() { return *strm_; }
    inline int level() { return level_; }

    ZlibDeflateHandle() = delete;
    ZlibDeflateHandle(const ZlibDeflateHandle &) = delete;
    ZlibDeflateHandle &operator=(const ZlibDeflateHandle &) = delete;

  private:
    z_stream_s *strm_;
    int level_;
  };
}  // namespace gs

#endif  // GENERS_ZLIBHANDLE_HH_
