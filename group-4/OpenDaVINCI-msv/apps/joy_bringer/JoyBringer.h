#ifndef __JOY_BRINGER__H__
#define __JOY_BRINGER__H__

#include <SDL.h>
#include "core/base/ConferenceClientModule.h"
#include "core/base/Mutex.h"
#include "GeneratedHeaders_Data.h"

namespace msv {

    using namespace std;

    class JoyBringer : public core::base::ConferenceClientModule {

        private:
            JoyBringer(const JoyBringer &/*obj*/);
            JoyBringer& operator=(const JoyBringer &/*obj*/);

            SDL_Joystick *joystick;
            SDL_Event event;

        public:
            JoyBringer(const int32_t &argc, char **argv);
            virtual ~JoyBringer();
            core::base::ModuleState::MODULE_EXITCODE body();

        private:
            virtual void setUp();
            virtual void tearDown();

            void setVehicle(const int&, const int&);
    };

}
    
#endif

