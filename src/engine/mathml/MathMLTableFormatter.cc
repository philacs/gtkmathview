// Copyright (C) 2000-2004, Luca Padovani <luca.padovani@cs.unibo.it>.
//
// This file is part of GtkMathView, a Gtk widget for MathML.
// 
// GtkMathView is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// GtkMathView is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GtkMathView; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// For details, see the GtkMathView World-Wide-Web page,
// http://helm.cs.unibo.it/mml-widget/, or send a mail to
// <lpadovan@cs.unibo.it>

#include <config.h>

#include <cassert>

#include "Adapters.hh"
#include "MathMLTableFormatter.hh"
#include "MathMLValueConversion.hh"
#include "FormattingContext.hh"
#include "MathGraphicDevice.hh"

MathMLTableFormatter::MathMLTableFormatter()
{ }

MathMLTableFormatter::~MathMLTableFormatter()
{ }

#include "BoundingBoxAux.hh"
#include "scaledAux.hh"

void
MathMLTableFormatter::init(const FormattingContext& ctxt,
			   unsigned nRows,
			   unsigned nColumns,
			   const std::vector<SmartPtr<MathMLTableCellElement> >& cell,
			   const std::vector<SmartPtr<MathMLTableCellElement> >& label,
			   const SmartPtr<Value>& columnWidthV,
			   const SmartPtr<Value>& rowSpacingV,
			   const SmartPtr<Value>& columnSpacingV,
			   const SmartPtr<Value>& frameV,
			   const SmartPtr<Value>& frameSpacingV,
			   const SmartPtr<Value>& equalColumnsV,
			   const SmartPtr<Value>& sideV,
			   const SmartPtr<Value>& minLabelSpacingV)
{
  const TokenId frame = ToTokenId(frameV);
  const TokenId side = ToTokenId(sideV);
  const bool hasFrame = frame == T_SOLID || frame == T_DASHED;
  const bool hasLabels = std::find_if(label.begin(), label.end(), NotNullPredicate<MathMLTableCellElement>()) != label.end();
  
  const unsigned nGridRows = (hasFrame ? 2 : 0) + nRows * 2 - 1;
  const unsigned nGridColumns = (hasFrame ? 2 : 0) + (hasLabels ? 4 : 0) + nColumns * 2 - 1;
  const unsigned contentColumnOffset = (hasFrame ? 1 : 0) + (hasLabels ? 2 : 0);
  const unsigned contentRowOffset = (hasFrame ? 1 : 0);
  const unsigned leftLabelOffset = (hasFrame ? 1 : 0);
  const unsigned rightLabelOffset = (hasFrame ? 1 : 0) + nColumns * 2 + 1;
  const unsigned labelOffset = (side == T_LEFT || side == T_LEFTOVERLAP) ? leftLabelOffset : rightLabelOffset;

  const bool equalColumns = ToBoolean(equalColumnsV);

  assert(nGridRows > 0);
  assert(nGridColumns > 0);

  std::cerr << "CI SIAMO: " << nRows << "x" << nColumns << std::endl
	    << "grid: " << nGridRows << "x" << nGridColumns << std::endl
	    << "frame? " << hasFrame << " labels? " << hasLabels << std::endl;

  std::vector<Row>(nGridRows).swap(rows);
  std::vector<Column>(nGridColumns).swap(columns);
  std::vector<Cell>(nGridRows * nGridColumns).swap(cells);

  std::cerr << "HAS FRAME?" << hasFrame << std::endl;
  if (hasFrame)
    {
      const scaled hFrameSpacing = ctxt.MGD()->evaluate(ctxt, resolveLength(ctxt, frameSpacingV, 0), scaled::zero());
      const scaled vFrameSpacing = ctxt.MGD()->evaluate(ctxt, resolveLength(ctxt, frameSpacingV, 1), scaled::zero());
      rows[0].setHeight(vFrameSpacing);
      rows[nGridRows - 1].setHeight(vFrameSpacing);
      columns[0].setWidthSpec(hFrameSpacing);
      columns[nGridColumns - 1].setWidthSpec(hFrameSpacing);
    }

  std::cerr << "HAS LABELS?" << hasLabels << std::endl;
  if (hasLabels)
    {
      const Length minLabelSpacing = resolveLength(ctxt, minLabelSpacingV);
      columns[leftLabelOffset].setWidthSpec(Column::FIT);
      columns[rightLabelOffset].setWidthSpec(Column::FIT);
      //columns[leftLabelOffset].setColumnAlign(...);
      //columns[rightLabelOffset].setColumnAlign(...);
      if (minLabelSpacing.type == Length::PERCENTAGE_UNIT)
	{
	  const float leftScaleSpacing = (side == T_LEFT || side == T_LEFTOVERLAP) ? minLabelSpacing.value : 0.0f;
	  const float rightScaleSpacing = (side == T_RIGHT || side == T_RIGHTOVERLAP) ? minLabelSpacing.value : 0.0f;
	  columns[leftLabelOffset + 1].setWidthSpec(leftScaleSpacing);
	  columns[rightLabelOffset - 1].setWidthSpec(rightScaleSpacing);
	}
      else
	{
	  const scaled leftFixSpacing = (side == T_LEFT || side == T_LEFTOVERLAP) ? ctxt.MGD()->evaluate(ctxt, minLabelSpacing, scaled::zero()) : scaled::zero();
	  const scaled rightFixSpacing = (side == T_RIGHT || side == T_RIGHTOVERLAP) ? ctxt.MGD()->evaluate(ctxt, minLabelSpacing, scaled::zero()) : scaled::zero();
	  columns[leftLabelOffset + 1].setWidthSpec(leftFixSpacing);
	  columns[rightLabelOffset - 1].setWidthSpec(rightFixSpacing);
	}
    }

  std::cerr << "SETUP COLUMNS" << std::endl;
  for (unsigned j = 0; j < nColumns; j++)
    {
      const unsigned jj = contentColumnOffset + j * 2;
      std::cerr << "setup column " << jj << std::endl;
      if (equalColumns)
	columns[jj].setWidthSpec(Column::AUTO);
      else
	{
	  const SmartPtr<Value> specV = GetComponent(columnWidthV, j);
	  if (isTokenId(specV, T_AUTO))
	    columns[jj].setWidthSpec(Column::AUTO);
	  else if (isTokenId(specV, T_FIT))
	    columns[jj].setWidthSpec(Column::FIT);
	  else
	    {
	      const Length spec = resolveLength(ctxt, specV);
	      if (spec.type == Length::PERCENTAGE_UNIT)
		columns[jj].setWidthSpec(spec.value / 100.0);
	      else
		columns[jj].setWidthSpec(ctxt.MGD()->evaluate(ctxt, spec, scaled::zero()));
	    }
	}
      columns[jj].setContentColumn();

      if (j + 1 < nColumns)
	{
	  const Length spacingSpec = resolveLength(ctxt, columnSpacingV, j);
	  if (spacingSpec.type == Length::PERCENTAGE_UNIT)
	    columns[jj + 1].setWidthSpec(spacingSpec.value);
	  else
	    columns[jj + 1].setWidthSpec(ctxt.MGD()->evaluate(ctxt, spacingSpec, scaled::zero()));
	}
    }

  std::cerr << "SETUP CONTENT" << std::endl;
  for (unsigned i = 0; i < nRows; i++)
    {
      const unsigned ii = contentRowOffset + i * 2;

      if (hasLabels)
	cells[ii * nGridColumns + labelOffset].setContent(label[i]);

      rows[ii].setHeight(Row::AUTO);
      for (unsigned j = 0; j < nColumns; j++)
	{
	  const unsigned jj = contentColumnOffset + j * 2;
	  cells[ii * nGridColumns + jj].setContent(cell[i * nColumns + j]);
	}
      rows[ii].setContentRow();

      if (i + 1 < nRows)
	{
	  const Length spacingSpec = resolveLength(ctxt, rowSpacingV, i);
	  if (spacingSpec.type == Length::PERCENTAGE_UNIT)
	    rows[ii + 1].setHeight(spacingSpec.value);
	  else
	    rows[ii + 1].setHeight(ctxt.MGD()->evaluate(ctxt, spacingSpec, scaled::zero()));
	}
    }
}

void
MathMLTableFormatter::formatCells(const FormattingContext& ctxt,
				  const scaled& availableWidth,
				  const SmartPtr<Value>& width) const
{
}

AreaRef
MathMLTableFormatter::formatLines(const FormattingContext& ctxt,
				  unsigned nRows,
				  unsigned nColumns,
				  const SmartPtr<Value>& frameV,
				  const SmartPtr<Value>& rowLinesV,
				  const SmartPtr<Value>& columnLinesV) const
{
  const TokenId frame = ToTokenId(frameV);
  const unsigned nGridRows = rows.size();
  const unsigned nGridColumns = columns.size();

  std::vector<BoxedLayoutArea::XYArea> content;
  for (unsigned ii = 0; ii < nGridRows; ii++)
    if (rows[ii].isContentRow())
      for (unsigned jj = 0; jj < nGridColumns; jj++)
	if (const Cell& cell = getCell(ii, jj))
	  {
	    const SmartPtr<MathMLTableCellElement> el = cell.getContent();
	    assert(el);
	    const unsigned i = el->getRowIndex();
	    const unsigned j = el->getColumnIndex();
	    std::cerr << "LINES: FOUND ELEMENT at " << i << "," << j << std::endl;
	    if (i + el->getRowSpan() < nRows && ToTokenId(GetComponent(rowLinesV, i)) != T_NONE)
	      {
		const scaled dx0 =
		  (j == 0) ?
		  ((frame != T_NONE) ? columns[jj - 1].getLeftDisplacement()
		   : columns[jj].getLeftDisplacement())
		  : columns[jj - 1].getCenterDisplacement();
		const scaled dx1 =
		  (j + el->getColumnSpan() == nColumns) ?
		  ((frame != T_NONE) ? columns[jj + cell.getColumnSpan()].getRightDisplacement()
		   : columns[jj + cell.getColumnSpan() - 1].getRightDisplacement())
		  : columns[jj + cell.getColumnSpan()].getCenterDisplacement();
		const scaled dy =
		  rows[ii + cell.getRowSpan()].getCenterDisplacement();
		BoxedLayoutArea::XYArea area(dx0, dy,
					     ctxt.MGD()->getFactory()->fixedHorizontalLine(ctxt.MGD()->defaultLineThickness(ctxt),
											   dx1 - dx0,
											   RGBColor::BLACK()));
		std::cerr << "draw x line " << dx0 << "," << dy << " to " << dx1 << std::endl;
		content.push_back(area);
	      }

	    if (j + el->getColumnSpan() < nColumns && ToTokenId(GetComponent(columnLinesV, j)) != T_NONE)
	      {
		const scaled dy0 =
		  (i == 0) ?
		  ((frame != T_NONE) ? rows[ii - 1].getTopDisplacement()
		   : rows[ii].getTopDisplacement())
		  : rows[ii - 1].getCenterDisplacement();
		const scaled dy1 =
		  (i + el->getRowSpan() == nRows) ?
		  ((frame != T_NONE) ? rows[ii + cell.getRowSpan()].getBottomDisplacement()
		   : rows[ii + cell.getRowSpan() - 1].getBottomDisplacement())
		  : rows[ii + cell.getRowSpan()].getCenterDisplacement();
		const scaled dx =
		  columns[jj + cell.getColumnSpan()].getCenterDisplacement();
		BoxedLayoutArea::XYArea area(dx, dy0,
					     ctxt.MGD()->getFactory()->fixedVerticalLine(ctxt.MGD()->defaultLineThickness(ctxt),
											 scaled::zero(),
											 dy0 - dy1,
											 RGBColor::BLACK()));
		std::cerr << "draw y line " << dx << "," << dy0 << " to " << dy1 << std::endl;
		content.push_back(area);
	      }
	  }

  if (frame != T_NONE)
    {
      std::cerr << "HAS FRAME" << std::endl;
    }

  return content.empty() ? 0 : ctxt.MGD()->getFactory()->boxedLayout(getBoundingBox(), content);
}

AreaRef
MathMLTableFormatter::format(const FormattingContext& ctxt,
			     unsigned nRows,
			     const SmartPtr<Value>& alignV,
			     const SmartPtr<Value>& equalRowsV,
			     const SmartPtr<Value>& equalColumnsV)
{
  std::cerr << "MathMLTableFormatter::format" << std::endl;

  const bool equalRows = ToBoolean(equalRowsV);
  const bool equalColumns = ToBoolean(equalColumnsV);

  std::cerr << "COMPUTING TABLE WIDTH" << std::endl;
  initTempWidths();
  if (equalColumns) initWidthsT();
  else initWidthsF();

  std::cerr << "COMPUTING TABLE HEIGHT" << std::endl;
  initTempHeights();
  const scaled tableHeightDepth = equalRows ? initHeightsT() : initHeightsF();

  std::cerr << "TABLE ALIGNMENT" << std::endl;
  const scaled& axis = ctxt.MGD()->axis(ctxt);
  TokenId align = ToTokenId(GetComponent(alignV, 0));
  if (IsEmpty(GetComponent(alignV, 1)))
    alignTable(tableHeightDepth, axis, align);
  else
    {
      const unsigned contentRowOffset = rows[0].isContentRow() ? 0 : 1;
      const int row = ToInteger(GetComponent(alignV, 1));
      const unsigned gridRow = contentRowOffset + 2 * ((row < 0) ? (nRows + row) : (row - 1));
      alignTable(tableHeightDepth, axis, align, gridRow);
    }
  std::cerr << "TABLE BBOX = " << BoundingBox(getWidth(), getHeight(), getDepth()) << std::endl;
  std::cerr << "SETTING ROW AND COLUMN DISPLACEMENTS" << std::endl;
  setDisplacements();
  std::cerr << "CELL POSITION" << std::endl;
  setCellPositions(axis);

  std::cerr << "TABLE AREA CREATION" << std::endl;
  std::vector<BoxedLayoutArea::XYArea> content;
  for (std::vector<Cell>::const_iterator p = cells.begin();
       p != cells.end();
       p++)
    if (!p->isNull())
      {
	scaled dx;
	scaled dy;
	p->getDisplacement(dx, dy);
	content.push_back(BoxedLayoutArea::XYArea(dx, dy, p->getArea()));
      }

  return ctxt.MGD()->getFactory()->boxedLayout(getBoundingBox(), content);
}

scaled
MathMLTableFormatter::getColumnContentWidth(unsigned j) const
{
  scaled maxWidth = 0;
  for (unsigned i = 0; i < rows.size(); i++)
    {
      if (rows[i].isContentRow())
	if (const Cell& cell = getCell(i, j))
	  if (cell.getColumnSpan() == 1)
	    maxWidth = std::max(maxWidth, cell.getBoundingBox().width);
    }
  std::cerr << "content width[" << j << "] = " << maxWidth << std::endl;
  return maxWidth;
}

void
MathMLTableFormatter::initTempWidths()
{
  for (unsigned j = 0; j < columns.size(); j++)
    {
      if (columns[j].isContentColumn() && columns[j].getSpec() != Column::FIX)
	{
	  const scaled contentWidth = getColumnContentWidth(j);
	  columns[j].setContentWidth(contentWidth);
	  columns[j].setTempWidth(contentWidth);
	}
      else if (columns[j].getSpec() == Column::FIX)
	columns[j].setTempWidth(columns[j].getFixWidth());
      else if (columns[j].getSpec() == Column::SCALE && !columns[j].isContentColumn())
	columns[j].setTempWidth(0);
    }

  for (unsigned j = 0; j < columns.size(); j++)
    for (unsigned i = 0; i < rows.size(); i++)
      if (rows[i].isContentRow() && columns[i].isContentColumn())
	if (const Cell& cell = getCell(i, j))
	  if (cell.getColumnSpan() > 1)
	    {
	      std::cerr << "CELL " << i << "," << j << " " << cell.getColumnSpan() << " " << cell.getBoundingBox() << std::endl;
	      const scaled cellWidth = cell.getBoundingBox().width;
	      scaled spannedTempWidth = 0;
	      for (unsigned z = j; z <= j + cell.getColumnSpan() - 1; z++)
		spannedTempWidth += columns[z].getTempWidth();
	      if (cellWidth > spannedTempWidth)
		for (unsigned z = j; z <= j + cell.getColumnSpan() - 1; z++)
		  if (columns[z].isContentColumn() && columns[j].getSpec() != Column::FIX)
		    columns[z].setTempWidth(columns[z].getTempWidth() + (cellWidth - spannedTempWidth) / static_cast<int>(cell.getColumnSpan()));
	    }
}

scaled
MathMLTableFormatter::calcTableWidthT(int& numCol, scaled& sumFix, float& sumScale)
{
  numCol = 0;
  sumFix = 0;
  sumScale = 0;

  scaled max = 0;                                                                                            
  for (unsigned j = 0; j < columns.size(); j++)
    {
      if (columns[j].isContentColumn())
	{
	  numCol++;
	  max = std::max(max, getColumnContentWidth(j));
	}
      else if (columns[j].getSpec() == Column::FIX)
	sumFix += columns[j].getFixWidth();
      else if (columns[j].getSpec() == Column::SCALE)
	sumScale += columns[j].getScaleWidth();
    }	
    
  return ((numCol * max) + sumFix) / (1 - sumScale);
}

void
MathMLTableFormatter::initWidthsT()
{
  int numCol;
  scaled sumFix;
  float sumScale;
	
  const scaled tableWidth = calcTableWidthT(numCol, sumFix, sumScale);

  const scaled assignedWidth = sumFix + tableWidth * sumScale;
  const scaled availWidth = tableWidth - assignedWidth;
	
  for (unsigned j = 0; j < columns.size(); j++)
    if (columns[j].isContentColumn())
      columns[j].setWidth(availWidth / numCol);
    else if (columns[j].getSpec() == Column::FIX)
      columns[j].setWidth(columns[j].getFixWidth());
    else if (columns[j].getSpec() == Column::SCALE)
      columns[j].setWidth(tableWidth * columns[j].getScaleWidth());

  setWidth(tableWidth);
}

scaled
MathMLTableFormatter::calcTableWidthF(int& numCol, scaled& sumCont, scaled& sumFix, float& sumScale)
{
  numCol = 0;
  sumCont = 0;
  sumFix = 0;
  sumScale = 0;
  scaled max = 0;
  scaled tempWidth = 0;
	
  for (unsigned j = 0; j < columns.size(); j++)
    if (columns[j].isContentColumn() && columns[j].getSpec() == Column::SCALE)
      max = std::max(max, getColumnContentWidth(j) / columns[j].getScaleWidth());

  for (unsigned j = 0; j < columns.size(); j++)
    if (columns[j].isContentColumn()
	&& columns[j].getSpec() != Column::FIX
	&& columns[j].getSpec() != Column::SCALE)
      {
	sumCont += columns[j].getTempWidth();
	numCol++;
      }
    else if (columns[j].getSpec() == Column::FIX)
      sumFix += columns[j].getFixWidth();
    else if (columns[j].getSpec() == Column::SCALE)
      sumScale += columns[j].getScaleWidth();

  return std::max(max, (sumCont + sumFix) / (1 - sumScale));
}

void
MathMLTableFormatter::initWidthsF()
{
  int numCol;
  scaled sumCont;
  scaled sumFix;
  float sumScale;

  const scaled tableWidth = calcTableWidthF(numCol, sumCont, sumFix, sumScale);

  const scaled assignedWidth = sumFix + tableWidth * sumScale;
  const scaled extraWidth = tableWidth - assignedWidth - sumCont;
			
  std::cerr << "initWidthsF sumScale = " << sumScale << std::endl
	  << "assignedWidth = " << assignedWidth << std::endl
	  << "extraWidth = " << extraWidth << std::endl;

  for (unsigned j = 0; j < columns.size(); j++)
    if ((columns[j].isContentColumn()
	 && columns[j].getSpec() != Column::FIX
	 && columns[j].getSpec() != Column::SCALE))
      columns[j].setWidth(columns[j].getContentWidth() + (extraWidth / numCol));
    else if (columns[j].getSpec() == Column::FIX)
      columns[j].setWidth(columns[j].getFixWidth());
    else if (columns[j].getSpec() == Column::SCALE)
      columns[j].setWidth(tableWidth * columns[j].getScaleWidth());

  setWidth(tableWidth);
}

void
MathMLTableFormatter::initTempHeights()
{
  for (unsigned i = 0; i < rows.size(); i++)
    {
      if (rows[i].getSpec() == Row::FIX)
	{
	  rows[i].setTempHeight(rows[i].getFixHeight());
	  rows[i].setTempDepth(0);
	}
      else if (rows[i].getSpec() == Row::SCALE)
	{
	  rows[i].setTempHeight(0);
	  rows[i].setTempDepth(0);
	}
      else if (rows[i].isContentRow())
	{
	  scaled maxH = 0;
	  scaled maxD = 0;
	  for (unsigned j = 0; j < columns.size(); j++)
	    if (const Cell& cell = getCell(i, j))
	      if (cell.getRowSpan() == 1 && cell.getRowAlign() == T_BASELINE)
		{
		  const BoundingBox box = cell.getBoundingBox();
		  maxH = std::max(maxH, box.height);
		  maxD = std::max(maxD, box.depth);
		}
	  rows[i].setTempHeight(maxH);
	  rows[i].setTempDepth(maxD);
	}
    }

  for (unsigned i = 0; i < rows.size(); i++)
    if (rows[i].isContentRow())
      for (unsigned j = 0; j < columns.size(); j++)
	if (const Cell& cell = getCell(i, j))
	  if (cell.getRowSpan() == 1 && cell.getRowAlign() != T_BASELINE)
	    {
	      const BoundingBox box = cell.getBoundingBox();
	      if ((rows[i].getTempHeight() + rows[i].getTempDepth()) < box.verticalExtent())
		rows[i].setTempDepth(box.verticalExtent() - rows[i].getTempHeight());
	    }
	
  for (unsigned i = 0; i < rows.size(); i++)
    for (unsigned j = 0; j < columns.size(); j++)
      if (columns[j].isContentColumn())
	if (const Cell& cell = getCell(i, j))
	  if (cell.getColumnSpan() > 1)
	    {
	      const scaled cellHeightDepth = cell.getBoundingBox().verticalExtent();
	      scaled spannedTempHeightDepth = 0;
	      for (unsigned z = i; z <= i + cell.getRowSpan() - 1; z++)
		spannedTempHeightDepth += rows[z].getTempHeight() + rows[z].getTempDepth();
	      if (cellHeightDepth > spannedTempHeightDepth)
		for (unsigned z = i; z <= i + cell.getRowSpan() - 1; z++)
		  if (rows[z].isContentRow())
		    rows[z].setTempDepth(rows[z].getTempDepth() + (cellHeightDepth - spannedTempHeightDepth) / static_cast<int>(cell.getRowSpan()));
	    }
}

scaled
MathMLTableFormatter::calcTableHeightDepthT(int& numRig, float& sumScale, scaled& sumContHD, scaled& sumFixHD)
{
  numRig = 0;
  sumScale = 0;
  scaled max = 0;
  sumContHD = 0;
  sumFixHD = 0;
	
  for (unsigned i = 0; i < rows.size(); i++)
    {
      if (rows[i].isContentRow())
	{
	  numRig++;
	  max = std::max(max, rows[i].getTempHeight() + rows[i].getTempDepth());
	  sumContHD += rows[i].getTempHeight() + rows[i].getTempDepth();
	}
      else if (rows[i].getSpec() == Row::FIX)
	sumFixHD += rows[i].getFixHeight();
      else if (rows[i].getSpec() == Row::SCALE)
	sumScale += rows[i].getScaleHeight();
    }

  return ((numRig * max) + sumFixHD) / (1 - sumScale);
}

scaled
MathMLTableFormatter::calcTableHeightDepthF()
{
  scaled sumContFix = 0;
  float sumScale = 0;

  for (unsigned i = 0; i < rows.size(); i++)
    if (rows[i].isContentRow() || rows[i].getSpec() == Row::FIX)
      sumContFix += rows[i].getTempHeight() + rows[i].getTempDepth();
    else if (rows[i].getSpec() == Row::SCALE)
      sumScale += rows[i].getScaleHeight();         

  return (sumContFix / (1 - sumScale));
}

scaled
MathMLTableFormatter::initHeightsT()
{
  int numRows;
  float sumScale;
  scaled sumFixHD;
  scaled sumContHD;
	
  const scaled tableHeightDepth = calcTableHeightDepthT(numRows, sumScale, sumContHD, sumFixHD);
	
  const scaled assignedHeightDepth = sumFixHD + sumContHD + tableHeightDepth * sumScale;
  const scaled availHeightDepth = tableHeightDepth - assignedHeightDepth;
	
  for (unsigned i = 0; i < rows.size(); i++)
    if (rows[i].isContentRow())
      {
	rows[i].setHeight(rows[i].getTempHeight());	
	rows[i].setDepth(availHeightDepth / numRows);
      }
    else if (rows[i].getSpec() == Row::FIX)
      {
	rows[i].setHeight(rows[i].getFixHeight());
	rows[i].setDepth(0);
      }
    else if (rows[i].getSpec() == Row::SCALE)
      {
	rows[i].setHeight(tableHeightDepth * rows[i].getScaleHeight());
	rows[i].setDepth(0);
      }

  return tableHeightDepth;
}			


scaled
MathMLTableFormatter::initHeightsF()
{
  const scaled tableHeightDepth = calcTableHeightDepthF();
	
  for (unsigned i = 0; i < rows.size(); i++)
    if (rows[i].isContentRow())
      {
	rows[i].setHeight(rows[i].getTempHeight());
	rows[i].setDepth(rows[i].getTempDepth());
      }
    else if (rows[i].getSpec() == Row::FIX)
      {
	rows[i].setHeight(rows[i].getFixHeight());
	rows[i].setDepth(0);
      }
    else if (rows[i].getSpec() == Row::SCALE)
      {
	rows[i].setHeight(tableHeightDepth * rows[i].getScaleHeight());
	rows[i].setDepth(0);
      }

  return tableHeightDepth;
}

void
MathMLTableFormatter::alignTable(const scaled& tableHeightDepth, const scaled& axis, TokenId align)
{
  switch (align)
    {
    case T_TOP:
      setHeight(0);
      break;
    case  T_BOTTOM:
      setHeight(tableHeightDepth);
      break;
    case T_AXIS:
      setHeight((tableHeightDepth / 2) + axis);
      break;
    case T_CENTER:
    case T_BASELINE:
      setHeight(tableHeightDepth / 2);
      break;
    default:
      assert(false);
    }

  setDepth(tableHeightDepth - getHeight());
}

void
MathMLTableFormatter::alignTable(const scaled& tableHeightDepth, const scaled& axis, TokenId align, unsigned rowNum)
{
  scaled temp = 0;

  for (unsigned i = 0; i < rowNum; i++)
    temp += rows[i].getVerticalExtent();

  switch (align)
    {
    case T_TOP:
      break;
    case T_BOTTOM:
      temp += rows[rowNum].getVerticalExtent();
      break;
    case T_AXIS:
      temp += rows[rowNum].getHeight() - axis;
      break;
    case T_CENTER:
      temp += rows[rowNum].getVerticalExtent() / 2;
      break;
    case T_BASELINE:
      temp += rows[rowNum].getHeight();
      break;
    default:
      assert(false);
    }
  setHeight(temp);

  setDepth(tableHeightDepth - getDepth());
}

BoundingBox
MathMLTableFormatter::getCellBoundingBox(unsigned i, unsigned j, unsigned rowSpan, unsigned columnSpan) const
{
  BoundingBox box(columns[j].getWidth(), rows[i].getHeight(), rows[i].getDepth());

  for (unsigned k = i + 1; k < i + rowSpan; k++)
    box.depth += rows[k].getVerticalExtent();

  for (unsigned k = j + 1; k < j + columnSpan; k++)
    box.width += columns[k].getWidth();

  return box;
}

void
MathMLTableFormatter::setDisplacements()
{
  scaled v = getHeight();
  for (unsigned i = 0; i < rows.size(); i++)
    {
      rows[i].setDisplacement(v - rows[i].getHeight());
      v -= rows[i].getVerticalExtent();
      std::cerr << "ROW[" << i << "].displacement = " << rows[i].getDisplacement() << std::endl;
    }

  scaled h = scaled::zero();
  for (unsigned j = 0; j < columns.size(); j++)
    {
      columns[j].setDisplacement(h);
      h += columns[j].getWidth();
      std::cerr << "COL[" << j << "].displacement = " << columns[j].getDisplacement() << std::endl;
    }
}

void
MathMLTableFormatter::setCellPositions(const scaled& axis)
{
  for (unsigned i = 0; i < rows.size(); i++)
    if (rows[i].isContentRow())
      for (unsigned j = 0; j < columns.size(); j++)
	if (columns[j].isContentColumn())
	  {
	    if (const Cell& cell = getCell(i, j))
	      {
		scaled dx = scaled::zero();
		scaled dy = scaled::zero();

		const BoundingBox box = cell.getBoundingBox();
		const BoundingBox cellBox = getCellBoundingBox(i, j, cell.getRowSpan(), cell.getColumnSpan());

		std::cerr << "CELL BOX = " << cellBox << std::endl << " CONTENT BOX = " << box << std::endl;

		switch (cell.getColumnAlign())
		  {
		  case T_LEFT:
		    dx = scaled::zero();
		    break;
		  case T_RIGHT:
		    dx = cellBox.width - box.width;
		    break;
		  case T_CENTER:
		    dx = (cellBox.width - box.width) / 2;
		    break;
		  default:
		    assert(false);
		  }

		switch (cell.getRowAlign())
		  {
		  case T_BASELINE:
		    dy = scaled::zero();
		    break;
		  case T_TOP:
		    dy = cellBox.height - box.height;
		    break;
		  case T_BOTTOM:
		    dy = box.depth - cellBox.depth;
		    break;
		  case T_CENTER:
		    dy = (cellBox.height - cellBox.depth - box.height + box.depth) / 2;
		    break;
		  case T_AXIS:
		    dy = (cellBox.height - cellBox.depth - box.height + box.depth) / 2 + axis;
		    break;
		  default:
		    assert(false);
		  }

		std::cerr << "setting displacement for (" << i << "," << j << ") = " << dx << "," << dy << std::endl;
		cell.setDisplacement(columns[j].getDisplacement() + dx, rows[i].getDisplacement() + dy);
	      }
	  }
}

const MathMLTableFormatter::Cell&
MathMLTableFormatter::getCell(unsigned i, unsigned j) const
{
  assert(i < rows.size());
  assert(j < columns.size());
  return cells[i * columns.size() + j];
}
