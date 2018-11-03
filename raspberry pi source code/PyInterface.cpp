#include "header.h"
#include "PyInterface.h"

bool PyStreamHandler::sendBGRImagePy(py::array_t<uint8_t, py::array::c_style | py::array::forcecast> array) {
   return sendBGRImage(array.data());
}

bool PyStreamHandler::sendRGBImagePy(py::array_t<uint8_t, py::array::c_style | py::array::forcecast> array) {
   return sendRGBImage(array.data());
}

bool PyStreamHandler::sendYUVImagePy(py::array_t<uint8_t, py::array::c_style | py::array::forcecast> array) {
   return sendYUVImage(array.data());
}

PYBIND11_MODULE(py_handler, m) {
   py::class_<PyStreamHandler> handler(m, "PyStreamHandler");
       handler
       .def(py::init<const int, const int, const int, const string&, const int, const int>())
       .def(py::init<const int, const int, const int, const string&, const int, const string&, const int, const int>())
       .def("sendBGRImage", &PyStreamHandler::sendBGRImagePy)
       .def("sendRGBImage", &PyStreamHandler::sendRGBImagePy)
       .def("sendYUVImage", &PyStreamHandler::sendYUVImagePy);
}
