#include <iostream>
#include <stdio.h>
#include <sys/time.h>

#include "core/macros.h"

#include "core/base/KeyValueConfiguration.h"
#include "core/data/Container.h"
#include "core/data/Constants.h"
#include "core/data/control/VehicleControl.h"
#include "core/data/environment/VehicleData.h"
#include "core/io/ContainerConference.h"
#include "core/wrapper/SharedMemoryFactory.h"
#include "core/wrapper/Time.h"

#include "GeneratedHeaders_Data.h"

#include "JoyBringer.h"

#define MAX(a,b) \
    ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })

#define MIN(a,b) \
    ({ __typeof__ (a) _a = (a); \
    __typeof__ (b) _b = (b); \
    _a < _b ? _a : _b; })

namespace msv {

    using namespace std;
    using namespace core::base;
    using namespace core::data;
    using namespace core::data::control;
    using namespace core::data::environment;

    using core::wrapper::Time;
    using core::wrapper::TimeFactory;

    JoyBringer::JoyBringer(const int32_t &argc, char **argv) :
        ConferenceClientModule(argc, argv, "joy_bringer"),
        joystick(),
        event()
        {}

    JoyBringer::~JoyBringer() {}

    void JoyBringer::setUp() {
        if (SDL_Init(SDL_INIT_JOYSTICK) < 0) {
            std::cout << SDL_GetError() << std::endl;
        }
        joystick = SDL_JoystickOpen(0);
        if (joystick == NULL) {
            fprintf(stderr, "SDL_JoystickOpen(%d) failed: %s\n", 0, SDL_GetError());
            //Fix crash
        }
    }

    void JoyBringer::tearDown() {
        SDL_JoystickClose(joystick);
    }

    void JoyBringer::setVehicle(const int& speed ,const int& value) {
        VehicleControl vc;
        vc.setSpeed(speed);
        int new_value = 0;
        if (value > 0) {
            new_value = MIN(value,45);
        } else if (value < 0) {
            new_value = MAX(value,-45);
        }
        vc.setSteeringWheelAngle(new_value * Constants::DEG2RAD);
        Container c(Container::VEHICLECONTROL, vc);
        getConference().send(c);
    }

    ModuleState::MODULE_EXITCODE JoyBringer::body()
    {

        Time *startTime = NULL;
        int steering = 0;
        int speed = 0;
        bool isReversing = false;
        bool isRevDown = false;

        while (getModuleState() == ModuleState::RUNNING) {
            while (SDL_PollEvent(&event)) {
                switch (event.type) {
                    case SDL_JOYAXISMOTION:
                        {
                            float procent = (event.jaxis.value/32767.0);
                            printf("Joystick %d axis %d value: %f\n",
                                   event.jaxis.which,
                                   event.jaxis.axis, procent);

                            if (event.jaxis.axis == 0) {
                                if (procent >= 0.1) {
                                    steering = 45;
                                } else if (procent <= -0.1) {
                                    steering = -45;
                                } else {
                                    steering = 0;
                                }
                            }
                            break;
                        }
                    case SDL_JOYBUTTONDOWN:
                        {
                            printf("Joystick %d button %d down\n",
                                   event.jbutton.which, event.jbutton.button);
                            if (SDL_JoystickNumButtons(joystick) >= 2) {
                                if (event.jbutton.button == 2) {
                                    if (!isRevDown)
                                       speed = 3;
                                } else if (event.jbutton.button == 1) {
                                    speed = -3;
                                    isRevDown = true;
                                } else if (event.jbutton.button == 9) {
                                    return ModuleState::OKAY;
                                }
                            } else {
                                return ModuleState::OKAY;
                            }
                            break;
                        }
                    case SDL_JOYBUTTONUP:
                        {
                            printf("Joystick %d button %d up\n",
                                   event.jbutton.which, event.jbutton.button);
                            if (event.jbutton.button == 2) {
                               speed = 0;
                            } else if (event.jbutton.button == 1) {
                               speed = 0;
                                isReversing = false;
                                delete startTime;
                                startTime = NULL;
                                isRevDown = false;
                            }
                            break;
                        }

                }

            }

            if ((speed < 0 && !isReversing) || isRevDown) {
                Time *t = TimeFactory::getInstance().now();

                if (startTime == NULL) {
                    startTime = t;
                    speed = -3;
                    std::cout << "Start reversing algorithm!" << std::endl;
                } else {
                    if ((t->getSeconds() - startTime->getSeconds()) <= 1) {
                        speed = -3;
                    } else if ((t->getSeconds() - startTime->getSeconds()) > 2) {
                        delete startTime;
                        startTime = NULL;
                        speed = -3;
                        isReversing = true;
                        isRevDown = false;
                        std::cout << "End reversing algorithm!" << std::endl;
                    } else {
                        speed = 0;
                    }
                    if (t != startTime) {
                        delete t;
                    }
                    t = NULL;
                }

            } else if (speed > 0 && isReversing) {
                isReversing = false;
                delete startTime;
                startTime = NULL;
                std::cout << "Aborted reversing algorithm!" << std::endl;
            }
            setVehicle(speed,steering);
        }
        return ModuleState::OKAY;
    }

}
