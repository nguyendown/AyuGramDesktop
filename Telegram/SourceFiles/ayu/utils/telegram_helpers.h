// This is the source code of AyuGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2023
#pragma once

#include "ayu/sync/models.h"

#include "core/application.h"
#include "main/main_account.h"
#include "main/main_domain.h"
#include "main/main_session.h"
#include "dialogs/dialogs_main_list.h"

using Callback = Fn<void(const QString&, UserData *)>;

Main::Session *getSession(ID userId);
bool accountExists(ID userId);
void dispatchToMainThread(std::function<void()> callback, int delay = 0);
not_null<History *> getHistoryFromDialogId(ID dialogId, Main::Session *session);
ID getDialogIdFromPeer(not_null<PeerData *> peer);
std::pair<std::string, std::string> serializeTextWithEntities(not_null<HistoryItem *> item);

ID getBareID(not_null<PeerData *> peer);

bool isAyuGramRelated(ID peerId);
bool isExteraRelated(ID peerId);

void MarkAsReadChatList(not_null<Dialogs::MainList *> list);
void MarkAsReadThread(not_null<Data::Thread *> thread);

void readHistory(not_null<HistoryItem *> message);

QString formatTTL(int time);
QString formatDateTime(const QDateTime &date);

QString getDCName(int dc);

QString getMediaSize(not_null<HistoryItem *> message);
QString getMediaMime(not_null<HistoryItem *> message);
QString getMediaName(not_null<HistoryItem *> message);
QString getMediaResolution(not_null<HistoryItem *> message);
QString getMediaDC(not_null<HistoryItem *> message);

void searchById(ID userId, Main::Session *session, bool retry, const Callback &callback);
void searchById(ID userId, Main::Session *session, const Callback& callback);