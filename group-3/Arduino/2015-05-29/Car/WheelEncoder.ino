//counting wheel revolutions - 8 ticks = 1 revolution. 1 revolution = 21 cm.
void wheelEncoder()
{
  if (digitalRead(encoderPinA) == digitalRead(encoderPinB)) {
    wheelTicks++;
  }
  else
    wheelTicks--;
}
