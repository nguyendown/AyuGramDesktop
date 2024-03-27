/*
This file is part of Telegram Desktop,
the official desktop application for the Telegram messaging service.

For license and copyright information please follow this link:
https://github.com/telegramdesktop/tdesktop/blob/master/LEGAL
*/
#include "menu/menu_sponsored.h"

#include "core/ui_integration.h" // Core::MarkedTextContext.
#include "data/data_premium_limits.h"
#include "data/data_session.h"
#include "data/stickers/data_custom_emoji.h"
#include "history/history.h"
#include "history/history_item.h"
#include "lang/lang_keys.h"
#include "main/main_session.h"
#include "ui/layers/generic_box.h"
#include "ui/painter.h"
#include "ui/rect.h"
#include "ui/text/text_utilities.h"
#include "ui/vertical_list.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/popup_menu.h"
#include "styles/style_channel_earn.h"
#include "styles/style_chat.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_premium.h"
#include "styles/style_settings.h"

namespace Menu {
namespace {

void AboutBox(
		not_null<Ui::GenericBox*> box,
		not_null<Main::Session*> session) {
	constexpr auto kUrl = "https://promote.telegram.org"_cs;

	box->setNoContentMargin(true);

	const auto content = box->verticalLayout().get();
	const auto levels = Data::LevelLimits(session)
		.channelRestrictSponsoredLevelMin();

	Ui::AddSkip(content);
	Ui::AddSkip(content);
	Ui::AddSkip(content);
	{
		const auto &icon = st::sponsoredAboutTitleIcon;
		const auto rect = Rect(icon.size() * 1.4);
		auto owned = object_ptr<Ui::RpWidget>(content);
		owned->resize(rect.size());
		const auto widget = box->addRow(object_ptr<Ui::CenterWrap<>>(
			content,
			std::move(owned)))->entity();
		widget->paintRequest(
		) | rpl::start_with_next([=] {
			auto p = Painter(widget);
			p.setPen(Qt::NoPen);
			p.setBrush(st::activeButtonBg);
			p.drawEllipse(rect);
			icon.paintInCenter(p, rect);
		}, widget->lifetime());
	}
	Ui::AddSkip(content);
	Ui::AddSkip(content);
	box->addRow(object_ptr<Ui::CenterWrap<>>(
		content,
		object_ptr<Ui::FlatLabel>(
			content,
			tr::lng_sponsored_menu_revenued_about(),
			st::boxTitle)));
	Ui::AddSkip(content);
	box->addRow(object_ptr<Ui::CenterWrap<>>(
		content,
		object_ptr<Ui::FlatLabel>(
			content,
			tr::lng_sponsored_revenued_subtitle(),
			st::channelEarnLearnDescription)));
	Ui::AddSkip(content);
	Ui::AddSkip(content);
	{
		const auto padding = QMargins(
			st::settingsButton.padding.left(),
			st::boxRowPadding.top(),
			st::boxRowPadding.right(),
			st::boxRowPadding.bottom());
		const auto addEntry = [&](
				rpl::producer<QString> title,
				rpl::producer<TextWithEntities> about,
				const style::icon &icon) {
			const auto top = content->add(
				object_ptr<Ui::FlatLabel>(
					content,
					std::move(title),
					st::channelEarnSemiboldLabel),
				padding);
			Ui::AddSkip(content, st::channelEarnHistoryThreeSkip);
			content->add(
				object_ptr<Ui::FlatLabel>(
					content,
					std::move(about),
					st::channelEarnHistoryRecipientLabel),
				padding);
			const auto left = Ui::CreateChild<Ui::RpWidget>(
				box->verticalLayout().get());
			left->paintRequest(
			) | rpl::start_with_next([=] {
				auto p = Painter(left);
				icon.paint(p, 0, 0, left->width());
			}, left->lifetime());
			left->resize(icon.size());
			top->geometryValue(
			) | rpl::start_with_next([=](const QRect &g) {
				left->moveToLeft(
					(g.left() - left->width()) / 2,
					g.top() + st::channelEarnHistoryThreeSkip);
			}, left->lifetime());
		};
		addEntry(
			tr::lng_sponsored_revenued_info1_title(),
			tr::lng_sponsored_revenued_info1_description(
				Ui::Text::RichLangValue),
			st::sponsoredAboutPrivacyIcon);
		Ui::AddSkip(content);
		Ui::AddSkip(content);
		addEntry(
			tr::lng_sponsored_revenued_info2_title(),
			tr::lng_sponsored_revenued_info2_description(
				Ui::Text::RichLangValue),
			st::sponsoredAboutSplitIcon);
		Ui::AddSkip(content);
		Ui::AddSkip(content);
		addEntry(
			tr::lng_sponsored_revenued_info3_title(),
			tr::lng_sponsored_revenued_info3_description(
				lt_count,
				rpl::single(float64(levels)),
				lt_link,
				tr::lng_settings_privacy_premium_link(
				) | rpl::map([=](QString t) {
					return Ui::Text::Link(t, kUrl.utf8());
				}),
				Ui::Text::RichLangValue),
			st::sponsoredAboutRemoveIcon);
		Ui::AddSkip(content);
		Ui::AddSkip(content);
	}
	Ui::AddSkip(content);
	Ui::AddSkip(content);
	{
		box->addRow(
			object_ptr<Ui::CenterWrap<Ui::FlatLabel>>(
				content,
				object_ptr<Ui::FlatLabel>(
					content,
					tr::lng_sponsored_revenued_footer_title(),
					st::boxTitle)));
	}
	Ui::AddSkip(content);
	{
		const auto arrow = Ui::Text::SingleCustomEmoji(
			session->data().customEmojiManager().registerInternalEmoji(
				st::topicButtonArrow,
				st::channelEarnLearnArrowMargins,
				false));
		const auto label = box->addRow(
			object_ptr<Ui::FlatLabel>(
				content,
				st::channelEarnLearnDescription));
		tr::lng_sponsored_revenued_footer_description(
			lt_link,
			tr::lng_channel_earn_about_link(
				lt_emoji,
				rpl::single(arrow),
				Ui::Text::RichLangValue
			) | rpl::map([=](TextWithEntities text) {
				return Ui::Text::Link(std::move(text), kUrl.utf8());
			}),
			Ui::Text::RichLangValue
		) | rpl::start_with_next([=, l = label](
				TextWithEntities t) {
			l->setMarkedText(
				std::move(t),
				Core::MarkedTextContext{
					.session = session,
					.customEmojiRepaint = [=] { l->update(); },
				});
			l->resizeToWidth(box->width()
				- rect::m::sum::h(st::boxRowPadding));
		}, label->lifetime());
	}
	Ui::AddSkip(content);
	Ui::AddSkip(content);
	{
		const auto &st = st::premiumPreviewDoubledLimitsBox;
		box->setStyle(st);
		auto button = object_ptr<Ui::RoundButton>(
			box,
			tr::lng_box_ok(),
			st::defaultActiveButton);
		button->resizeToWidth(box->width()
			- st.buttonPadding.left()
			- st.buttonPadding.left());
		button->setClickedCallback([=] { box->closeBox(); });
		box->addButton(std::move(button));
	}

}

} // namespace

void ShowSponsored(
		not_null<Ui::RpWidget*> parent,
		std::shared_ptr<Ui::Show> show,
		not_null<HistoryItem*> item) {
	Expects(item->isSponsored());

	struct State final {
	};
	const auto state = std::make_shared<State>();

	const auto menu = Ui::CreateChild<Ui::PopupMenu>(
		parent.get(),
		st::popupMenuWithIcons);

	menu->addAction(tr::lng_sponsored_menu_revenued_about(tr::now), [=] {
		show->show(Box(AboutBox, &item->history()->session()));
	}, &st::menuIconInfo);

	menu->popup(QCursor::pos());
}

} // namespace Menu