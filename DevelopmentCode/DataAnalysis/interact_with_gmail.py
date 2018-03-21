from __future__ import print_function
import poplib
from email import parser
import os
from execute_bash_command import subprocess_cmd


def download_message_attachment(message_in):
    attachment = message_in.get_payload()[1]
    attachment_string = attachment.get_payload(decode=True)
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


def extract_infos_message(parsed_message, verbose=0):
    information_from = parsed_message['From']
    information_date = parsed_message['Date']

    if verbose > 0:
        print("From {}".format(information_from))
        print("Date {}".format(information_date))

    return(information_from, information_date)


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

        if self.verbose > 0:
            print("Loaded {} messages".format(len(self.messages)))

        # disconnect
        pop_conn.quit()

    def generate_data_all_incoming_messages(self):
        self.parsed_messages = []

        crrt_message_index = 0

        for crrt_message in self.messages:

            crrt_message_index += 1
            if self.verbose > 0:
                print("generate data for message number {}".format(crrt_message_index))

            parsed_message = parse_message(crrt_message)
            self.parsed_messages.append(parsed_message)
            (information_from, information_date) = extract_infos_message(parsed_message)
            iridium_id = information_from.rsplit(' ', 1)[0]
            message_name = information_date.replace(" ", "_").replace(",", "")
            attachment_string = download_message_attachment(parsed_message)

            path_data = self.path_repo + '/' + iridium_id + '/'

            if not os.path.exists(path_data):
                os.makedirs(path_data)

            if self.verbose > 0:
                print("save data for message number {}".format(crrt_message_index))

            with open(path_data + message_name + '.bin', 'wb') as crrt_file:
                crrt_file.write(attachment_string)

    def push_data(self):
        if self.verbose > 0:
            print("Commit and push to github")

        subprocess_cmd("cd {} && git add . && git commit -m 'automatic commit' && git push".format(self.path_repo))
