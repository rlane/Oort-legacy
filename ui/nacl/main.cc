#include "gl/gl.h"
#include "ppapi/cpp/instance.h"
#include "ppapi/cpp/module.h"
#include "ppapi/gles2/gl2ext_ppapi.h"

class OortModule : public pp::Module {
 public:
  OortModule() : pp::Module() {}

  virtual ~OortModule() {
    glTerminatePPAPI();
  }

  virtual bool Init() {
    return glInitializePPAPI(get_browser_interface()) == GL_TRUE;
  }

  virtual pp::Instance* CreateInstance(PP_Instance instance) {
    return NULL;
  }
};

namespace pp {

Module* CreateModule() {
  return new OortModule();
}

}
