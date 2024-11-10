#ifndef LOGIN_PAGE_H
#define LOGIN_PAGE_H

#include "session.h"
#include "utility.h"

void login_page(struct Session* session);

enum UserCommand get_login_page_command();
int do_login();
int do_signup();
int do_admin();

#endif
