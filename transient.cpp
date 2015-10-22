#include "transient.h"

Transient::Transient(Parser *_parser, double _dt, double _t2, double _t1) {
    dt = _dt;
    t1 = _t1;
    t2 = _t2;
}

Transient::~Transient() {

}
