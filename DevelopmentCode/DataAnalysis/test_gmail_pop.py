from __future__ import print_function
import poplib
from email import parser

"""
to check the status of less secure apps:

wheel > settings > Accounts and imports > Other google account settings > Signing in to google > Allow less secur app on

TODO: automatically push on github the content of the messages
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

messages

"""
# Concat message pieces:
messages = ["\n".join(mssg[1]) for mssg in messages]

# Parse message intom an email object:
messages = [parser.Parser().parsestr(mssg) for mssg in messages]
"""

messages[0]

"\n".join(messages[0][1])

current_message = parser.Parser().parsestr("\n".join(messages[0][1]))
current_message['To']
current_message['Date']
current_message['From']
print(current_message)

current_message['From']

current_message.keys()

print(current_message)

attachment = current_message.get_payload()[1]

attachment.get_payload(decode=False)

pop_conn.list()
pop_conn.stat()

connection_list = pop_conn.list()
number_of_messages = len(connection_list[1])
print("number of messages: {}".format(number_of_messages))
for crrt_message in connection_list[1]:
    print("message number | message size: {}".format(crrt_message))

pop_conn.list()[0]
pop_conn.list()[1]
pop_conn.list()[1][0][0]
pop_conn.list()[1][0]

pop_conn.quit()
