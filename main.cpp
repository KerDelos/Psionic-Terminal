#include <thread>


#include "ftxui/component/screen_interactive.hpp"
#include "ftxui/component/container.hpp"
#include "ftxui/screen/string.hpp"

using namespace ftxui;

class PsionicMainScreenComponent : public Component {

 public:
  ~PsionicMainScreenComponent() override {}
  PsionicMainScreenComponent(){}


  Element Render() override {
    return text(L"Welcome to Psionic with FTXUI") | hcenter;
  }
};


int main(int argc, const char* argv[]) {
  auto screen = ScreenInteractive::Fullscreen();

  std::thread update([&screen]() {
    for (;;) {
      using namespace std::chrono_literals;
      std::this_thread::sleep_for(0.05s);
      screen.PostEvent(Event::Custom);
    }
  });

  PsionicMainScreenComponent main_screen;
  screen.Loop(&main_screen);

  return 0;
}
