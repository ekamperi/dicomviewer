#include "selectable.h"

Selectable::Selectable()
{
    /* By default an item, is not selected */
    this->selected = false;
}

void Selectable::setSelection(bool selected)
{
    this->selected = selected;
}

bool Selectable::isSelected(void) const
{
    return this->selected;
}
