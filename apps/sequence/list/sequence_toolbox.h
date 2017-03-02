#ifndef SEQUENCE_SEQUENCE_TOOLBOX_H
#define SEQUENCE_SEQUENCE_TOOLBOX_H

#include "../../math_toolbox.h"

namespace Sequence {

class SequenceToolbox : public MathToolbox {
public:
  SequenceToolbox();
  bool handleEvent(Ion::Events::Event event) override;
  int numberOfRows() override;
  HighlightCell * reusableCell(int index, int type) override;
  void willDisplayCellForIndex(HighlightCell * cell, int index) override;
  KDCoordinate rowHeight(int j) override;
  int typeAtLocation(int i, int j) override;
  void setExtraCells(const char * sequenceName, int recurrenceDepth);
private:
  bool selectAddedCell(int selectedRow);
  int mathToolboxIndex(int index);
  constexpr static KDCoordinate k_addedRowHeight = 20;
  ExpressionTableCell m_addedCells[k_maxNumberOfDisplayedRows];
  Poincare::ExpressionLayout * m_addedCellLayout[k_maxNumberOfDisplayedRows];
  int m_numberOfAddedCells;
};

}

#endif
