import typing
import operator
import abc

from functools import reduce

class VisitorInterface(metaclass=abc.ABCMeta):
    @classmethod
    def __subclasshook__(cls, subclass):
        return (hasattr(subclass, 'visitVariable') and callable(subclass.visitVariable) and
                hasattr(subclass, 'visitConstant') and callable(subclass.visitConstant) and
                hasattr(subclass, 'visitAdd') and callable(subclass.visitAdd) and
                hasattr(subclass, 'visitMultiply') and callable(subclass.visitMultiply) and
                hasattr(subclass, 'visitSubtract') and callable(subclass.visitSubtract) and
                hasattr(subclass, 'visitDivide') and callable(subclass.visitDivide))

class ExpressionInterface(metaclass=abc.ABCMeta):
    @classmethod
    def __subclasshook__(cls, subclass):
        def apply(self, visitor: VisitorInterface):
            return (hasattr(subclass, 'apply') and callable(subclass.apply))

class Variable(ExpressionInterface):
    def __init__(self, name):
        self._name = name

    def getName(self):
        return self._name

    def apply(self, visitor: VisitorInterface):
        return visitor.visitVariable(self)

class Constant(ExpressionInterface):
    def __init__(self, value):
        self._value = value

    def getValue(self):
        return self._value

    def apply(self, visitor: VisitorInterface):
        return visitor.visitConstant(self)

class BinaryExpression(ExpressionInterface):
    def __init__(self, child1: ExpressionInterface, child2: ExpressionInterface):
        self._child1 = child1
        self._child2 = child2

    def getChild1(self):
        return self._child1

    def getChild2(self):
        return self._child2

    def apply(self, visitor):
        raise NotImplementedException()

class Add(BinaryExpression):
    def apply(self, visitor):
        return visitor.visitAdd(self)

class Multiply(BinaryExpression):
    def apply(self, visitor):
        return visitor.visitMultiply(self)

class Subtract(BinaryExpression):
    def apply(self, visitor):
        return visitor.visitSubtract(self)

class Divide(BinaryExpression):
    def apply(self, visitor):
        return visitor.visitDivide(self)

class EvaluationVisitor(VisitorInterface):
    def __init__(self, variable_values):
        self._variable_values = variable_values

    def visitVariable(self, var_expression: Variable):
        return self._variable_values[var_expression.getName()]

    def visitConstant(self, const_expression: Constant):
        return const_expression.getValue()

    def visitAdd(self, add_expression: Add):
        return add_expression.getChild1().apply(self) + add_expression.getChild2().apply(self) 

    def visitMultiply(self, expression: Multiply):
        return expression.getChild1().apply(self) * expression.getChild2().apply(self)

    def visitSubtract(self, expression: Subtract):
        return expression.getChild1().apply(self) - expression.getChild2().apply(self)

    def visitDivide(self, expression: Divide):
        return expression.getChild1().apply(self) // expression.getChild2().apply(self)

if __name__ == "__main__":
    visitor = EvaluationVisitor({'x': 5, 'y': 7, 'z': 3})

    print('Assuming: x = 5, y = 7 and z = 3.')
    print()

    print('Constants:')
    expression = Constant(1)
    print(f'1 = {expression.apply(visitor)}')
    assert(expression.apply(visitor) == 1)
    expression = Constant(5)
    print(f'5 = {expression.apply(visitor)}')
    assert(expression.apply(visitor) == 5)
    print()

    print('Variables:')
    expression = Variable('x')
    print(f'x = {expression.apply(visitor)}')
    assert(expression.apply(visitor) == 5)
    expression = Variable('y')
    print(f'y = {expression.apply(visitor)}')
    assert(expression.apply(visitor) == 7)
    expression = Variable('z')
    print(f'z = {expression.apply(visitor)}')
    assert(expression.apply(visitor) == 3)
    print()

    print('Simple examples:')
    expression = Add(Constant(1), Constant(2))
    print(f'1 + 2 = {expression.apply(visitor)}')
    assert(expression.apply(visitor) == 3)
    expression = Multiply(Variable('x'), Constant(5))
    print(f'x * 5 = {expression.apply(visitor)}')
    assert(expression.apply(visitor) == 25)
    expression = Divide(Constant(7), Variable('z'))
    print(f'7 / z = {expression.apply(visitor)}')
    assert(expression.apply(visitor) == 2)
    expression = Subtract(Variable('y'), Variable('z'))
    print(f'y - z = {expression.apply(visitor)}')
    assert(expression.apply(visitor) == 4)
    print()

    print('More complex examples:')
    expression = Subtract(Multiply(Add(Constant(1), Constant(2)), Add(Variable('x'), Constant(3))), Constant(7))
    print(f'(1 + 2) * (x + 3) - 7 = {expression.apply(visitor)}')
    assert(expression.apply(visitor) == 17)
    expression = Divide(Multiply(Add(Variable('z'), Constant(7)), Add(Variable('y'), Constant(3))), Subtract(Multiply(Constant(5), Variable('x')), Constant(4)))
    print(f'((z + 7) * (y + 3)) / (5 * x - 4) = {expression.apply(visitor)}')
    assert(expression.apply(visitor) == 4)

