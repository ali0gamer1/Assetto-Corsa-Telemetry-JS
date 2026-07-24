from pycparser import c_parser, c_ast



def DEBUG_PRINT(msg):
    print(f"DEBUG: {type(msg)}: {msg}")
    
ARRAY_DIM_DELIMETER = "." #Delimeter to separate the dimensions in the parser

#TODO: handle user created types and struct in structs
class BaseTypeClass: #Or primitive
    
    def __init__(self, name) -> None:
        
        self.name = name

    
    def __repr__(self) -> str:
        
        return self.name
    



class ArrayType(BaseTypeClass):
    
    def __init__(self, name, dims) -> None:
        
        super().__init__(name)

        self.dims = dims
    
    def __str__(self) -> str:
        return f"{self.name}{self.getDimensions(self.dims)}"
    
    
    

    def getDimensions(self, dims):
        retVal = ""
        for dim in dims:
                retVal = retVal +  f"[{dim}]"
        
    
        return retVal


def read_C_file(filename):
    with open(filename, "r") as f:
        return f.read()


def getDataTypeSize(dataType):

        type_sizes = {
            "char": 1,
            "short": 2,
            "int": 4,
            "long": 8,
            "float": 4,
            "double": 8,
        }
        
        return type_sizes.get(dataType, None) 





class StructVisitor(c_ast.NodeVisitor):
    
    def visit_Struct(self, node):
        if node.decls is not None and isinstance(node, c_ast.Struct):
            
            print(f"Found the struct: {node.name}")
            
            offset = 0
            nextOffset = 0
            
            self.fieldsMetadata = dict()
            
            for field in node.decls:
                
                field_name = field.name
                field_type = self._get_type(field.type)
                
                
                print("===============")
                
                print(f"{field_name}, {field_type}")
                
                print("===============")
                
                self.fieldsMetadata[field_name] = {
                    "size": getDataTypeSize
                    
                    }
                
                      
                
            pass

    
    def _get_type(self, type_node, lastType = None):
        
        if isinstance(type_node, c_ast.TypeDecl):
            return self._get_type(type_node.type, lastType)
        elif isinstance(type_node, c_ast.IdentifierType):
            if (lastType is not None):
                # TODO: return an object instead of string
                # return F"[]{ARRAY_DIM_DELIMETER}" + " ".join(type_node.names) + ARRAY_DIM_DELIMETER +\
                # ARRAY_DIM_DELIMETER.join(lastType)
                
                return ArrayType(" ".join(type_node.names), lastType) 
                
                
            return " ".join(type_node.names)
        
        elif isinstance(type_node,c_ast.PtrDecl):
            return f"{self._get_type(type_node.type, lastType)}*" #star at the end

        elif isinstance(type_node,c_ast.ArrayDecl):
            dim = f"{type_node.dim.value}"
            if lastType is not None:
                lastType.append(dim)
            else:
                lastType = [dim]
            return self._get_type(type_node.type, lastType=lastType)

        
        return "unknown"
        

parser = c_parser.CParser()
visitor = StructVisitor()

tree = parser.parse(read_C_file('./AcPhysics.h'))

visitor.visit(tree)