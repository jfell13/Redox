#ifndef VELOCITY_H
#define VELOCITY_H

class Velocity {
public:
    Velocity(double x, double y, double z);
    double getX() const;
    double getY() const;
    double getZ() const;
private:
    double x_, y_, z_;
};

#endif // VELOCITY_H
