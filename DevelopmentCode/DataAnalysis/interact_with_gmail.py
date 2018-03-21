from __future__ import print_function
import poplib
from email import parser


def download_message_attachment(message_in):
    attachment = message_in.get_payload()[1]
    attachment_string = attachment.get_payload(decode=False)
    return(attachment_string)


def parse_message(message_in):
    parsed_message = parser.Parser().parsestr("\n".join(message_in[1]))

    """
    NOTE:
        at this stage, easy to get some information such as:
        - From: parsed_message['From']
        - Date: parsed_message['Date']
    """

    return(parsed_message)


"""
TODO:
when read data, save it to disk on the location of a github repo, and then push the repo
organize the repo by sensor ID (from the from field)
name messages from dates of arrival
in a separate, highler level class: bundle together GPS and spectra data
"""

class IridiumEmailReader(object):
    def __init__(self, username, password, path_repo, verbose=0):
        self.username = username
        self.password = password
        self.path_repo = path_repo
        self.verbose = verbose

    def read_all_incoming_messages(self):
        # connect
        pop_conn = poplib.POP3_SSL('pop.gmail.com')
        pop_conn.user(self.username)
        pop_conn.pass_(self.password)

        # read messages
        self.messages = [pop_conn.retr(i) for i in range(1, len(pop_conn.list()[1]) + 1)]

        # disconnect
        pop_conn.quit()

    def generate_data_all_incoming_messages(self):
        """todo: generate the data and save it"""
        pass

    def push_data(self):
        """todo: push to github"""
        pass
