class AnalyzeStream(object):

    def __init__(self, string_to_find, verbose=0):
        self.string_to_find = string_to_find
        self.length_message = len(self.string_to_find)
        self.current_position = 0
        self.found_message = False

    def read_char(self, char_in):
        if char_in == self.string_to_find[self.current_position]:
            self.current_position += 1
        else:
            self.current_position = 0

        if self.current_position == self.length_message:
            self.found_message = True
            return(True)
        else:
            return(False)

    def has_found_message(self):
        return(self.found_message)
