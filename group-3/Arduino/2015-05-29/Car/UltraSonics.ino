#include <Wire.h>
#include <SonarSRF08.h>

SonarSRF08 sonarCentre;
SonarSRF08 sonarRight;
//setup ultra sonic rangefinders
void setupSonars() {
  sonarCentre.connect(sonarCentreAddress, GAIN_REGISTER, LOCATION_REGISTER);
  sonarRight.connect(sonarRightAddress, GAIN_REGISTER, LOCATION_REGISTER);
}

//get readings from ultra sonic
void getSonarReadings() {
  sonarCentreReading = sonarCentre.getRange();
  if (sonarCentreReading > 0) {
    sonarCentreReading = constrain(sonarCentreReading, sonarMin, sonarMax);
    sonarCentreTemp = sonarCentreReading;
  } else {
    sonarCentreReading = sonarCentreTemp;
  }
  sonarRightReading = sonarRight.getRange();
  if (sonarRightReading > 0) {
    sonarRightReading = constrain(sonarRightReading, sonarMin, sonarMax);
    sonarRightTemp = sonarRightReading;
  } else {
    sonarRightReading = sonarRightTemp;
  }
}
