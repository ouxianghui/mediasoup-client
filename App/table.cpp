#include "table.h"

#ifndef MAX
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#endif

void combine(TableCell& a, TableCell& b, TableCell& comb) {
    comb.r1 = MIN(a.r1, b.r1);
    comb.r2 = MAX(a.r2, b.r2);
    comb.c1 = MIN(a.c1, b.c1);
    comb.c2 = MAX(a.c2, b.c2);
}

bool overlap(TableCell& a, TableCell& b, TableCell& inter, TableCell& comb) {
    inter.r1 = MAX(a.r1, b.r1);
    inter.r2 = MIN(a.r2, b.r2);
    inter.c1 = MAX(a.c1, b.c1);
    inter.c2 = MIN(a.c2, b.c2);

    if (inter.r1 < inter.r2 && inter.c1 < inter.c2) {
        combine(a, b, comb);
        return true;
    }

    return false;
}

Table::Table() {
    row = col = 0;
}

void Table::init(int row, int col) {
    this->row = row;
    this->col = col;
    _mapCells.clear();
    for (int r = 1; r <= row; ++r) {
        for (int c = 1; c <= col; ++c) {
            int id = (r-1) * col + c;
            _mapCells[id] = TableCell(r, r+1, c, c+1);
        }
    }
}

bool Table::getTableCell(int id, TableCell& rst) {
    if (_mapCells.find(id) != _mapCells.end()) {
        rst = _mapCells[id];
        return true;
    }
    return false;
}

TableCell Table::merge(int lt, int rb) {
    TableCell cell_lt, cell_rb, inter, comb;
    if (getTableCell(lt, cell_lt) && getTableCell(rb, cell_rb)) {
        combine(cell_lt, cell_rb, comb);

        std::map<int, TableCell>::iterator iter = _mapCells.begin();
        while (iter != _mapCells.end()) {
            if (overlap(comb, iter->second, inter, comb)) {
                iter = _mapCells.erase(iter);
            }
            else {
                ++iter;
            }
        }

        int id = (comb.r1-1) * col + comb.c1;
        _mapCells[id] = comb;

        return comb;
    }

    return TableCell();
}
