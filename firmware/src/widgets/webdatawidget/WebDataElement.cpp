#include "WebDataElement.h"

bool WebDataElement::isChanged() {
    return m_changed;
}
void WebDataElement::setChangedStatus(bool changed) {
    m_changed = changed;
}
