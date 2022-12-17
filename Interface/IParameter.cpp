#include "IParameter.h"

IParameter::IParameter() {}

double IParameter::timeBase() const { return m_timeBase; }

void IParameter::setTimeBase(double newTimeBase) { m_timeBase = newTimeBase; }
