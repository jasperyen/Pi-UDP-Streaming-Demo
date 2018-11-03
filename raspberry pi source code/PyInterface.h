#include "header.h"
#include "StreamHandler.h"

#ifndef PYINTERFACE_H
#define PYINTERFACE_H

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

namespace py = pybind11;

class PyStreamHandler: public StreamHandler {
public:
   using StreamHandler::StreamHandler;
   bool sendYUVImagePy(py::array_t<uint8_t, py::array::c_style | py::array::forcecast>);
   bool sendBGRImagePy(py::array_t<uint8_t, py::array::c_style | py::array::forcecast>);
   bool sendRGBImagePy(py::array_t<uint8_t, py::array::c_style | py::array::forcecast>);
};

#endif
