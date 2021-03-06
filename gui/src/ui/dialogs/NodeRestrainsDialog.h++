#ifndef NODE_RESTRAINS_DIALOG_HPP
#define NODE_RESTRAINS_DIALOG_HPP


#include "ui/ui_NodeRestrainDialog.h"

#include <libfemp/NodeRestrictions.h++>


class NodeRestrainsDialog
		: public QDialog, private Ui_NodeRestrainDialog
{
public:
	enum Restraints {
		NONE=0,
		RX = 0x01,
		RY = 0x02,
		RZ = 0x04
	};

public:
	explicit NodeRestrainsDialog(QWidget *parent = nullptr);
	~NodeRestrainsDialog();

	int getRestraints();

	fem::NodeRestrictions getRestrictions() const;

};


#endif
