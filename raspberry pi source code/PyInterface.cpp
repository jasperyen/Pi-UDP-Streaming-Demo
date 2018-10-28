#include "header.h"
#include "StreamHandler.h"

PYBIND11_MODULE(py_handler, m) {
   py::class_<StreamHandler> handler(m, "PyStreamHandler");
       handler
       .def(py::init<const int, const int, const int, const string&, const int, const int>())
       .def(py::init<const int, const int, const int, const string&, const int, const string&, const int, const int>())
       .def("sendBGRImage", &StreamHandler::sendBGRImagePy)
       .def("sendRGBImage", &StreamHandler::sendRGBImagePy)
       .def("sendYUVImage", &StreamHandler::sendYUVImagePy);
}
