#include "GroupNode.h"

#include "ControlGroup.h"
#include "SoftControl.h"

using namespace MaximRuntime;

GroupNode::GroupNode(Schematic *parent) : Node(parent), _subsurface(parent->runtime(), parent, parent->depth() + 1) {

}

GroupNode::~GroupNode() = default;

void GroupNode::remove() {
    for (const auto &control : _controls) {
        control->remove();
    }

    Node::remove();
}

void GroupNode::compile() {
    instFunc()->reset();
    instFunc()->addInstantiable(_subsurface.instFunc());
    instFunc()->complete();

    CompileUnit::compile();
}

void GroupNode::addControl(std::unique_ptr<SoftControl> control) {
    assert(control->node() == this);

    auto controlPtr = control.get();
    _controls.push_back(std::move(control));

    connect(controlPtr, &SoftControl::cleanup,
            [this, controlPtr]() { removeControl(controlPtr); });

    emit controlAdded(controlPtr);
}

void GroupNode::removeControl(SoftControl *control) {
    for (auto i = _controls.begin(); i < _controls.end(); i++) {
        if (i->get() == control) {
            _controls.erase(i);
            return;
        }
    }
}
