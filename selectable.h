#ifndef SELECTABLE_H
#define SELECTABLE_H

class Selectable
{
public:
    Selectable();
    void setSelection(bool selected);
    bool isSelected(void) const;
private:
    bool selected;
};

#endif // SELECTABLE_H
