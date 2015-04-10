#include "Lines.h"

Lines::Lines(double x, double y, double crit) :
	xPos(x),
	yPos(y),
	critical(crit) {}
	

double Lines::getXPos()
{
	return xPos;
}

double Lines::getYPos()
{
	return yPos;
}

void Lines::setXPos(double x)
{
	xPos = x;
}
void Lines::setYPos(double y)
{
	yPos = y;
}
double Lines::getCritical()
{
	return critical;
}

