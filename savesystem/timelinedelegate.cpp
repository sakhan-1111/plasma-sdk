/***************************************************************************
 *   Copyright (C) 2007 by Pino Toscano <pino@kde.org>                     *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/

// Stolen from okular :-)

#include	<QApplication>
#include	<QAbstractItemDelegate>
#include	<QPainter>
#include	<QStyleOptionViewItem>
#include	<QModelIndex>
#include	<QSize>

#include	"timelinedelegate.h"

/* A simple delegate to paint the icon of each item */
static const int ITEM_MARGIN_LEFT		= 5;
static const int ITEM_MARGIN_TOP		= 5;
static const int ITEM_MARGIN_RIGHT		= 5;
static const int ITEM_MARGIN_BOTTOM		= 5;
static const int ITEM_PADDING			= 5;



TimeLineDelegate::TimeLineDelegate( QObject *parent )
	: QAbstractItemDelegate( parent ), m_showText( true )
{
}

TimeLineDelegate::~TimeLineDelegate()
{
}

void TimeLineDelegate::setShowText( bool show )
{
	m_showText = show;
}

bool TimeLineDelegate::isTextShown() const
{
	return m_showText;
}

void TimeLineDelegate::paint( QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	QBrush backBrush;
	QColor foreColor;
	bool disabled = false;
	bool hover = false;
	if ( !( option.state & QStyle::State_Enabled ) )
	{
		backBrush = option.palette.brush( QPalette::Disabled, QPalette::Base );
		foreColor = option.palette.color( QPalette::Disabled, QPalette::Text );
		disabled = true;
	}
	else if ( option.state & ( QStyle::State_HasFocus | QStyle::State_Selected ) )
	{
		backBrush = option.palette.brush( QPalette::Highlight );
		foreColor = option.palette.color( QPalette::HighlightedText );
	}
	else if ( option.state & QStyle::State_MouseOver )
	{
		backBrush = option.palette.color( QPalette::Highlight ).light( 115 );
		foreColor = option.palette.color( QPalette::HighlightedText );
		hover = true;
	}
	else /*if ( option.state & QStyle::State_Enabled )*/
	{
		backBrush = option.palette.brush( QPalette::Base );
		foreColor = option.palette.color( QPalette::Text );
	}
	QStyle *style = QApplication::style();
	QStyleOptionViewItemV4 opt( option );
	// KStyle provides an "hover highlight" effect for free;
	// but we want that for non-KStyle-based styles too
	if ( !style->inherits( "KStyle" ) && hover )
	{
		Qt::BrushStyle bs = opt.backgroundBrush.style();
		if ( bs > Qt::NoBrush && bs < Qt::TexturePattern )
			opt.backgroundBrush = opt.backgroundBrush.color().light( 115 );
		else
			opt.backgroundBrush = backBrush;
	}
	painter->save();
	style->drawPrimitive( QStyle::PE_PanelItemViewItem, &opt, painter, 0 );
	painter->restore();
	QIcon icon = index.data( Qt::DecorationRole ).value< QIcon >();
	if ( !icon.isNull() )
	{
		QPoint iconpos(
			( option.rect.width() - option.decorationSize.width() ) / 2,
			ITEM_MARGIN_TOP
		);
		iconpos += option.rect.topLeft();
		QIcon::Mode iconmode = disabled ? QIcon::Disabled : QIcon::Normal;
		painter->drawPixmap( iconpos, icon.pixmap( option.decorationSize, iconmode ) );
	}

	if ( m_showText )
	{
		QString text = index.data( Qt::DisplayRole ).toString();
		QRect fontBoundaries = QFontMetrics( option.font ).boundingRect( text );
		QPoint textPos(
			ITEM_MARGIN_LEFT + ( option.rect.width() - ITEM_MARGIN_LEFT - ITEM_MARGIN_RIGHT - fontBoundaries.width() ) / 2,
			ITEM_MARGIN_TOP + option.decorationSize.height() + ITEM_PADDING
		);
		fontBoundaries.translate( -fontBoundaries.topLeft() );
		fontBoundaries.translate( textPos );
		fontBoundaries.translate( option.rect.topLeft() );
		painter->setPen( foreColor );
		painter->drawText( fontBoundaries, Qt::AlignCenter, text );
	}
}

QSize TimeLineDelegate::sizeHint( const QStyleOptionViewItem &option, const QModelIndex &index ) const
{
	QSize baseSize( option.decorationSize.width(), option.decorationSize.height() );
	if ( m_showText )
	{
		QRect fontBoundaries = QFontMetrics( option.font ).boundingRect( index.data( Qt::DisplayRole ).toString() );
		baseSize.setWidth( qMax( fontBoundaries.width(), baseSize.width() ) );
		baseSize.setHeight( baseSize.height() + fontBoundaries.height() + ITEM_PADDING );
	}
	return baseSize + QSize( ITEM_MARGIN_LEFT + ITEM_MARGIN_RIGHT, ITEM_MARGIN_TOP + ITEM_MARGIN_BOTTOM );
}