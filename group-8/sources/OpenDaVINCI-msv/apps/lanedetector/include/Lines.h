

#ifndef LINES_H_
#define LINES_H_

    class Lines{
        public:
	       
	        Lines(double x, double y, double crit);

	        double getXPos();
	        double getYPos();
	        double getCritical();
	        void setXPos(double x);
	        void setYPos(double y);
	        void setCritical(double crit);

        private:
            double xPos;
			double yPos;
			double critical;


    };


#endif /*LINES_H_*/