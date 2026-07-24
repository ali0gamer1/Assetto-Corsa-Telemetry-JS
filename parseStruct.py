from pycparser import c_parser, c_ast,parse_file
from functools import reduce

#TODO: handle user created types and struct in structs
class BaseTypeClass: #Or primitive
    
    def __init__(self, name = "", basesize = 0) -> None:
        
        self.basesize = basesize
        self.name = name

    def getTotalSize(self):
        return self.basesize

    def __str__(self) -> str:
        return self.name
    
    def __repr__(self) -> str:
        return self.name
        


class ArrayType(BaseTypeClass):
    
    def __init__(self, dims) -> None:
        
        super().__init__()

        self.dims = dims
        self.totalsize = 0
        
    
    def getTotalSize(self):
            return self.totalsize
    
    def __repr__(self) -> str:
            return f"{self.name}{self.getDimensions(self.dims)}"
        
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


def getDataTypeSize(dataType)->int:

        type_sizes = {
            "char": 1,
            "short": 2,
            "int": 4,
            "long": 8,
            "float": 4,
            "double": 8,
        }
        
        return type_sizes.get(dataType, -1) 





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
                                
                self.fieldsMetadata[field_name] = {"typedata":field_type,
                                                   "offset": offset}
                
                nextOffset += field_type.getTotalSize()
                offset = nextOffset
            print(self.fieldsMetadata)      
                
    
    def _get_type(self, type_node, lastType = None)->BaseTypeClass:
        
        if isinstance(type_node, c_ast.TypeDecl):
            return self._get_type(type_node.type, lastType)
        
        elif isinstance(type_node, c_ast.IdentifierType):
            
            
            if type(lastType) is ArrayType:
                lastType.name = " ".join(type_node.names)
                lastType.basesize = getDataTypeSize(lastType.name) # type: ignore
                lastType.totalsize = lastType.basesize * reduce((lambda x,y: x + y ) , lastType.dims)
                return lastType
            
            _name = " ".join(type_node.names)
            return BaseTypeClass(name = _name, basesize = getDataTypeSize(_name))
        
        elif isinstance(type_node,c_ast.ArrayDecl):

            
            dim = type_node.dim.value
            
            if isinstance(lastType, ArrayType):
                lastType.dims.append(int(dim))
            else:
                lastType = ArrayType(dims = [int(dim)])
            
                
            return self._get_type(type_node.type, lastType=lastType)

        
        return BaseTypeClass()
        

visitor = StructVisitor()

fake_include_path = r'.\utils\fake_libc_include'

ast = parse_file(
    'AcPhysics.h',
    use_cpp=True,
    cpp_path=r'cpp', 
    cpp_args=[
        '-E',  
        '-nostdinc',  
        f'-I{fake_include_path}'  
    ]) # type: ignore
visitor.visit(ast)