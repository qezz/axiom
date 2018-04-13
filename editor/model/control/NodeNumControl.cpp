#include <editor/model/node/Node.h>
#include "NodeNumControl.h"

#include "../schematic/Schematic.h"
#include "../node/NodeSurface.h"
#include "../Project.h"
#include "../history/ChangeNumValueOperation.h"
#include "compiler/runtime/Control.h"
#include "compiler/runtime/ControlGroup.h"
#include "compiler/runtime/Node.h"
#include "compiler/runtime/Runtime.h"

using namespace AxiomModel;

NodeNumControl::NodeNumControl(Node *node, MaximRuntime::Control *runtime, QPoint pos, QSize size)
    : NodeControl(node, runtime, pos, size), m_sink(this) {
    initSink();

    connect(&m_sink, &NumConnectionSink::valueChanged,
            this, &NodeNumControl::valueChanged);
}

void NodeNumControl::doRuntimeUpdate() {
    saveValue();
}

void NodeNumControl::startSetValue() {
    beforeVal = value();
}

void NodeNumControl::setValue(MaximRuntime::NumValue value, bool setRuntime) {
    m_sink.setValue(value);
    if (setRuntime) restoreValue();
}

void NodeNumControl::endSetValue() {
    if (beforeVal != value()) {
        node->parentSchematic->project()->history.appendOperation(
            std::make_unique<ChangeNumValueOperation>(node->parentSchematic->project(), ref(), beforeVal, value()));
    }
}

void NodeNumControl::saveValue() {
    if (!runtime()->group()) return;
    setValue(runtime()->group()->getNumValue(), false);
}

void NodeNumControl::restoreValue() {
    if (!runtime()->group()) return;
    runtime()->group()->setNumValue(m_sink.value());
}

void NodeNumControl::setMode(Mode mode) {
    if (mode != m_mode) {
        m_mode = mode;
        emit modeChanged(mode);
    }
}

void NodeNumControl::setChannel(Channel channel) {
    if (channel != m_channel) {
        m_channel = channel;
        emit channelChanged(channel);
    }
}

void NodeNumControl::serialize(QDataStream &stream) const {
    stream << m_sink.value();
}

void NodeNumControl::deserialize(QDataStream &stream) {
    MaximRuntime::NumValue val; stream >> val;
    m_sink.setValue(val);
}
