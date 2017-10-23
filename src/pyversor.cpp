#include <pyversor/pyversor.h>

namespace pyversor {

PYBIND11_MODULE(pyversor_, m) {
  ega::add_submodule(m);
  cga::add_submodule(m);
}

} // namespace pyversor
