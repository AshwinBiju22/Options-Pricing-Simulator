#include <pybind11/pybind11.h>
#include <pybind11/stl.h>          // auto-convert std::vector <-> Python list
#include "order_book.hpp"

namespace py = pybind11;
using namespace qe;

PYBIND11_MODULE(engine, m) {
    m.doc() = "C++ limit-order-book matching engine";

    py::enum_<Side>(m, "Side")
        .value("Buy",  Side::Buy)
        .value("Sell", Side::Sell);

    py::enum_<OrderType>(m, "OrderType")
        .value("Limit",  OrderType::Limit)
        .value("Market", OrderType::Market);

    py::class_<Order>(m, "Order")
        .def_readonly("id",       &Order::id)
        .def_readonly("side",     &Order::side)
        .def_readonly("type",     &Order::type)
        .def_readonly("price",    &Order::price)
        .def_readonly("quantity", &Order::quantity);

    py::class_<Trade>(m, "Trade")
        .def_readonly("aggressor_id", &Trade::aggressor_id)
        .def_readonly("resting_id",   &Trade::resting_id)
        .def_readonly("price",        &Trade::price)
        .def_readonly("quantity",     &Trade::quantity)
        .def("__repr__", [](const Trade& t) {
            return "<Trade " + std::to_string(t.quantity) + " @ " + std::to_string(t.price) + ">";
        });

    py::class_<SubmitResult>(m, "SubmitResult")
        .def_readonly("order_id", &SubmitResult::order_id)
        .def_readonly("trades",   &SubmitResult::trades);

    py::class_<OrderBook>(m, "OrderBook")
        .def(py::init<>())
        .def("submit", &OrderBook::submit,
             py::arg("side"), py::arg("type"), py::arg("price"), py::arg("quantity"))
        .def("cancel", &OrderBook::cancel, py::arg("id"))
        .def("best_bid", &OrderBook::best_bid, py::return_value_policy::reference_internal)
        .def("best_ask", &OrderBook::best_ask, py::return_value_policy::reference_internal)
        .def_readonly("bids", &OrderBook::bids)
        .def_readonly("asks", &OrderBook::asks);
}