import poplib
from email import parser

"""
to check the status of less secure apps:

wheel > settings > Accounts and imports > Other google account settings > Signing in to google > Allow less secur app on
"""

pop_conn = poplib.POP3_SSL('pop.gmail.com')

"""
username = raw_input("enter gmail username: ")
pop_conn.user(username)

password = raw_input("enter gmail password: ")
pop_conn.pass_(password)
"""

pop_conn.user("iridium.uio.math.mech")
pop_conn.pass_("hJ742@))+=h")

# Get messages from server:
messages = [pop_conn.retr(i) for i in range(1, len(pop_conn.list()[1]) + 1)]

# Concat message pieces:
messages = ["\n".join(mssg[1]) for mssg in messages]

# Parse message intom an email object:
messages = [parser.Parser().parsestr(mssg) for mssg in messages]
for message in messages:
    print message['subject']

pop_conn.quit()
