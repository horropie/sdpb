#include "is_valid_char.hxx"
#include "../../Positive_Matrix_With_Prefactor_State.hxx"

#include <algorithm>
#include <iterator>
#include <string>

inline void check_iterator(const std::vector<char>::const_iterator &iterator,
                           const std::vector<char>::const_iterator &end)
{
  if(iterator == end)
    {
      throw std::runtime_error("Invalid polynomial string");
    }
}

std::vector<char>::const_iterator
parse_polynomial(const std::vector<char>::const_iterator &begin,
                 const std::vector<char>::const_iterator &end,
                 Polynomial &polynomial)
{
  const std::vector<char> delimiters({',', '}'});
  const auto delimiter(
    std::find_first_of(begin, end, delimiters.begin(), delimiters.end()));
  if(delimiter == end)
    {
      throw std::runtime_error("Missing '}' at end of array of polynomials");
    }

  std::string mantissa;
  for(auto c(begin); c < delimiter; ++c)
    {
      if(*c == '`')
        {
          do
            {
              ++c;
            }
          while(c != delimiter
                && (std::isdigit(*c) || *c == '.' || !is_valid_char(*c)
                    || *c == '`'));
        }
      if(*c == '*')
        {
          do
            {
              ++c;
              check_iterator(c, delimiter);
            }
          while(!is_valid_char(*c));

          std::string exponent;
          if(*c == '^')
            {
              exponent = "E";
              ++c;
              check_iterator(c, delimiter);
              while((exponent.size() == 1 && (*c == '-' || *c == '+'))
                    || std::isdigit(*c) || !is_valid_char(*c))
                {
                  if(is_valid_char(*c))
                    {
                      exponent.push_back(*c);
                    }
                  ++c;
                  check_iterator(c, delimiter);
                }
              while(c != delimiter && (!is_valid_char(*c) || *c == '*'))
                {
                  ++c;
                }
            }

          size_t degree(0);
          // Hard code the polynomial to be in 'x' since that is what
          // SDPB.m uses.
          if(*c == 'x')
            {
              ++c;
              while(!is_valid_char(*c))
                {
                  ++c;
                  check_iterator(c, delimiter);
                }
              if(*c != '^')
                {
                  degree = 1;
                }
              else
                {
                  ++c;
                  std::string degree_string;
                  while((degree_string.empty() && (*c == '-' || *c == '+'))
                        || std::isdigit(*c) || !is_valid_char(*c))
                    {
                      if(is_valid_char(*c) && *c != '+')
                        {
                          degree_string.push_back(*c);
                        }
                      ++c;
                    }
                  degree = std::stoull(degree_string);
                }
            }

          if(polynomial.coefficients.size() < degree + 1)
            {
              polynomial.coefficients.resize(degree + 1);
            }
          polynomial.coefficients.at(degree)
            = El::BigFloat(mantissa + exponent);
          mantissa.clear();
        }
      else if(!mantissa.empty() && (*c == '-' || *c == '+'))
        {
          if(polynomial.coefficients.size() < 1)
            {
              polynomial.coefficients.resize(1);
            }
          polynomial.coefficients.at(0) = El::BigFloat(mantissa);
          mantissa.clear();
        }
      if(is_valid_char(*c) && *c != '+')
        {
          mantissa.push_back(*c);
        }
    }
  return delimiter;
}
