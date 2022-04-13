#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_map>

#include "fc/static_variant.hpp"

namespace expr {

class constant;
class variable;

template <typename op>
class binary_expression;

typedef binary_expression<std::plus<int>> add;
typedef binary_expression<std::multiplies<int>> multiply;
typedef binary_expression<std::minus<int>> subtract;
typedef binary_expression<std::divides<int>> divide;

typedef fc::static_variant<constant, variable, add, multiply, subtract, divide>
    expression;

class evaluation_visitor;

class constant {
  const int value;

 public:
  constant(int _value) : value(_value) {}

  int get_value() const { return this->value; }
};

class variable {
  std::string name;

 public:
  variable(std::string _name) : name(_name) {}

  std::string get_name() const { return name; }
};

template <typename op_type>
class binary_expression {
  std::unique_ptr<expression> child1;
  std::unique_ptr<expression> child2;

 public:
  static constexpr op_type op = op_type();

  template <typename expr_type_1, typename expr_type_2>
  binary_expression(expr_type_1 &&_child1, expr_type_2 &&_child2) {
    child1 = std::make_unique<expr::expression>(_child1);
    child2 = std::make_unique<expr::expression>(_child2);
  }

  binary_expression<op_type>(const binary_expression<op_type> &A) {
    child1 = std::make_unique<expr::expression>(*A.child1);
    child2 = std::make_unique<expr::expression>(*A.child2);
  }

  binary_expression<op_type>(binary_expression<op_type> &&A) {
    child1 = std::move(A.child1);
    child2 = std::move(A.child2);
  }

  binary_expression<op_type> &operator=(const binary_expression<op_type> &A) {
    child1 = std::make_unique<expr::expression>(*A.child1);
    child2 = std::make_unique<expr::expression>(*A.child2);
    return *this;
  }

  binary_expression<op_type> &operator=(binary_expression<op_type> &&A) {
    child1 = std::move(A.child1);
    child2 = std::move(A.child2);
    return *this;
  }

  const std::unique_ptr<expression> &get_child1() const { return child1; }
  const std::unique_ptr<expression> &get_child2() const { return child2; }
};

class evaluation_visitor {
  std::unordered_map<std::string, int> var_evaluation;

 public:
  typedef int result_type;

  evaluation_visitor(std::unordered_map<std::string, int> &&_var_evaluation)
      : var_evaluation(_var_evaluation) {}

  int operator()(constant const &e) { return e.get_value(); }

  int operator()(variable const &e) { return var_evaluation[e.get_name()]; }

  template <typename op_type>
  int operator()(binary_expression<op_type> const &e) {
    return binary_expression<op_type>::op(e.get_child1()->visit(*this),
                                          e.get_child2()->visit(*this));
  }
};

}  // namespace expr

int main() {
  expr::evaluation_visitor visitor{{{"x", 5}, {"y", 7}, {"z", 3}}};

  std::cout << "Assuming: x = 5, y = 7 and z = 3.\n\n";

  std::cout << "Constants:\n";
  std::unique_ptr<expr::expression> example_expr =
      std::make_unique<expr::expression>(expr::constant(5));
  std::cout << "5 = " << example_expr->visit(visitor) << "\n";
  example_expr = std::make_unique<expr::expression>(expr::constant(4));
  std::cout << "4 = " << example_expr->visit(visitor) << "\n\n";

  std::cout << "Variables:\n";
  example_expr = std::make_unique<expr::expression>(expr::variable("x"));
  std::cout << "x = " << example_expr->visit(visitor) << "\n";
  example_expr = std::make_unique<expr::expression>(expr::variable("y"));
  std::cout << "y = " << example_expr->visit(visitor) << "\n";
  example_expr = std::make_unique<expr::expression>(expr::variable("z"));
  std::cout << "z = " << example_expr->visit(visitor) << "\n\n";

  std::cout << "Simple examples:\n";
  example_expr = std::make_unique<expr::expression>(
      expr::add(expr::constant(1), expr::constant(2)));
  std::cout << "1 + 2 = " << example_expr->visit(visitor) << "\n";
  example_expr = std::make_unique<expr::expression>(
      expr::multiply(expr::variable("x"), expr::constant(5)));
  std::cout << "x * 5 = " << example_expr->visit(visitor) << "\n";
  example_expr = std::make_unique<expr::expression>(
      expr::divide(expr::constant(7), expr::variable("z")));
  std::cout << "7 / z = " << example_expr->visit(visitor) << "\n";
  example_expr = std::make_unique<expr::expression>(
      expr::subtract(expr::variable("y"), expr::variable("z")));
  std::cout << "y - z = " << example_expr->visit(visitor) << "\n\n";

  std::cout << "More complex examples:\n";
  example_expr = std::make_unique<expr::expression>(expr::subtract(
      expr::multiply(expr::add(expr::constant(1), expr::constant(2)),
                     expr::add(expr::variable("x"), expr::constant(3))),
      expr::constant(7)));
  std::cout << "(1 + 2) * (x + 3) - 7 = " << example_expr->visit(visitor)
            << "\n";
  example_expr = std::make_unique<expr::expression>(expr::divide(
      expr::multiply(expr::add(expr::variable("z"), expr::constant(7)),
                     expr::add(expr::variable("y"), expr::constant(3))),
      expr::subtract(expr::multiply(expr::constant(5), expr::variable("x")),
                     expr::constant(4))));
  std::cout << "((z + 7) * (y + 3)) / (5 * x - 4) = "
            << example_expr->visit(visitor) << "\n\n";
}
