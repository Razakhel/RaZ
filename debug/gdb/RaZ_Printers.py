import gdb
import re

############
# Printers #
############

# Raz::Vector<T, Size>
class RazVectorPrinter:
    def __init__(self, val):
        self.val    = val
        self.values = self.val['m_data']['_M_elems']

    def to_string(self):
        return self.values

    def display_hint(self):
        return 'array'

# Raz::Matrix<T, W, H>
class RazMatrixPrinter:
    def __init__(self, val):
        self.val    = val
        self.values = self.val['m_data']['_M_elems']

    def to_string(self):
        width  = self.val.type.template_argument(1)
        height = self.val.type.template_argument(2)

        printStr = "{"

        for heightIndex in range(0, height):
            printStr += "{ "

            for widthIndex in range(0, width - 1):
                printStr += str(self.values[heightIndex * width + widthIndex]) + ", "
            printStr += str(self.values[heightIndex * width + width - 1]) + " }, "

        printStr = printStr[:-4] + " }}"

        return printStr

    def display_hint(self):
        return 'array'

##############
# Dispatcher #
##############

def DispatchFromType(val):
    # unqualified() removes type qualifiers (const & volatile)
    # strip_typedefs() removes aliases (for example, Raz::Vec3f will be transformed to its real type Raz::Vector<float, 3>)

    typeStr = str(val.type.unqualified().strip_typedefs())

    if re.match("^Raz::Vector<.*, \d*>$", typeStr):
        return RazVectorPrinter(val)
    elif re.match("^Raz::Matrix<.*, \d*, \d*>$", typeStr):
        return RazMatrixPrinter(val)

    return None

gdb.pretty_printers.append(DispatchFromType)
