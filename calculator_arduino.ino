#include <ArduinoSTL.h>
#include <Key.h>
#include <Keypad.h>
#include <vector>
#include <String>
#include <cmath>
#include <LiquidCrystal.h>
#include <Keypad.h>

//using std::String;

#define INPUTMODE 1
#define OUTPUTMODE 2

#define LEFTPARENTHESIS 50
#define RIGHTPARENTHESIS 52


void printVectors(std::vector<String> vc)
{
  for(int i = 0; i < vc.size(); i++)
  {
    if(vc.at(i) != "(" || vc.at(i) != ")")
      Serial.print( vc.at(i)+" " );
  }
  Serial.println();
}

double getPrior(char c)
{
  if(c == '^')
    return 3;
  if(c == '*' || c == '/')
    return 2;
  if(c == '+' || c == '-')
    return 1;
}

bool isOperator(String c)
{
  return c == "+" || c == "-" || c == "*" || c == "/" || c == "^";
}

bool isOperator(char c)
{
  return c == '+' || c == '-' || c == '*' || c == '/' || c == '^';
}

bool isOperand(String c)
{
  if(!(isOperator(c)))
    return (c != ")") && (c != "(");
  return false;
}

double calculateWithSign(char c, double a, double b)
{
  switch(c)
  {
    case '+':
    return (a + b);
    case '-':
    return (a - b);
    case '*':
    return (a * b);
    case '/':
    return (a / b);
  case '^':
    return pow(a, b);
  }
}

double postfixCalculator(std::vector<String> postfixes)
{
  std::vector<double> operands;
  double a = 0, b = 0, c = 0;
  for(double i = 0; i < postfixes.size(); i++)
  {
    if(isOperand(postfixes[i])) {
      String postfix = postfixes[i];
      operands.push_back(postfix.toFloat());
    } else
    if(operands.size()>1)
    {
      b = operands.back();
      operands.pop_back();
      a = operands.back();
      operands.pop_back();
      c = calculateWithSign(postfixes[i][0], a, b);
      operands.push_back(c);
    }
  }
  return operands[0];
}

std::vector<String> convertToPostfix(std::vector<String> infix)
{
  std::vector<String> operators;
  std::vector<String> postfixes;

  for(double i = 0; i < infix.size(); i++)
  {
    if(isOperand(infix[i]))
      postfixes.push_back(infix[i]);

    else if(infix[i] == "(")
    {
      operators.push_back(infix[i]);
    }

    else if(infix[i] == ")")
    {
      while(!operators.empty() && operators.back()[0] !=  '(')
      {
        String temp = operators.back();
        postfixes.push_back(temp);
        operators.pop_back();
      }
      operators.pop_back();
    }

    else if(isOperator(infix[i]))
    {
      while (!operators.empty() && operators.back()[0] != '(' && getPrior(operators.back()[0]) >= getPrior(infix[i][0]))
      {
        String temp = operators.back();
        postfixes.push_back(temp);
        operators.pop_back();
      }
      operators.push_back(infix[i]);
    }
  }

  while (operators.size()!=0)
  {
    String temp = operators.back();
    postfixes.push_back(temp);
    operators.pop_back();
  }
  return postfixes;
}

std::vector<String> tokenizer(String str)
{
  String tempstr="";
  std::vector<String> vc;
  for(double i = 0; i < str.length(); i++)
  {
    if(str[i]==' ')
      continue;
    if(str[i]=='('||str[i]==')')
    {
      Serial.println("Found ()");
      tempstr = str[i];
      vc.push_back(tempstr);
      tempstr = "";
      continue;
    }

    if(isOperator(str[i]))
    {
      tempstr = "";
      tempstr += str[i];
      vc.push_back(tempstr);
      tempstr = "";
    }
    else
    {
      while(i < str.length())
      {
        tempstr += str[i];
        i++;
        if(isOperator(str[i])||str[i]=='('||str[i]==')')
        {
          i--;
          break;
        }
      }
      vc.push_back(tempstr);
      tempstr = "";
    }
  }
  return vc;
}


const byte ROWS = 4; 
const byte COLS = 4; 

char hexaKeys[ROWS][COLS] = {
  {'1', '2', '3', '+'},
  {'4', '5', '6', '-'},
  {'7', '8', '9', '/'},
  {'*', '0', '=', 'C'}
};
// A = +
//B = -
//C = /
// D = CLR
// # = =
byte rowPins[ROWS] = {53, 51, 49, 47}; 
byte colPins[COLS] = {45, 43, 41, 39}; 

Keypad customKeypad = Keypad(makeKeymap(hexaKeys), rowPins, colPins, ROWS, COLS); 
String tempStr= "";
double tempInt;
char op;
double num1, num2 = -1;
double result;
bool equal = false;





LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

void setup() {
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);
  //lcd.print("Hello World");
  Serial.begin(9600);
  pinMode(LEFTPARENTHESIS, INPUT);
  pinMode(RIGHTPARENTHESIS, INPUT);
}

int leftBracket=0;
int rightBracket=0;

int mode=INPUTMODE;
String inputStr="";
double solution;

void loop() {
  // put your main code here, to run repeatedly:
  leftBracket = digitalRead(LEFTPARENTHESIS);
  rightBracket = digitalRead(RIGHTPARENTHESIS);
  char customKey = customKeypad.getKey();
  if(leftBracket==HIGH)
  {
    customKey='(';
    delay(200);
  }
  if(rightBracket==HIGH)
  {
    customKey=')';
    delay(200);
  }
  if (customKey || leftBracket==HIGH || rightBracket==HIGH){
    if (customKey != '=')
    {
      mode = INPUTMODE;
    }
    else {
      mode = OUTPUTMODE;
    }
  
    switch(mode){
      case INPUTMODE:
        if(customKey!='C' && customKey!='=')
        {
          inputStr+=customKey;
        }else
        if(customKey=='C')
        {
          inputStr="";
        }
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print(inputStr);
        break;
        
       case OUTPUTMODE:
        lcd.setCursor(0,0);
        lcd.print(inputStr);
        Serial.println(inputStr);
        printVectors(tokenizer(inputStr));
        printVectors(convertToPostfix(tokenizer(inputStr)));
        solution = postfixCalculator(convertToPostfix(tokenizer(inputStr)));
        lcd.setCursor(0,1);
        lcd.print(solution);
        //mode = INPUTMODE;
        break;
    }
  }

  
 /* if (customKey){
    //lcd.print(customKey);
    //Serial.print(customKey);
    if (customKey>='0' && customKey <='9'){
      tempStr+=customKey;
      Serial.println(tempStr);
      lcd.clear();
      lcd.setCursor(0,1);
      lcd.print(tempStr.toInt());
    }
    else
    {
      tempInt = tempStr.toFloat();
      tempStr="";
      if(num1==-1)
        num1=tempInt;
      else
        num2=tempInt;
      op = customKey;
    }

    if(num1!= -1 && num2!= -1)
    {
      //calculation
      switch(op){
        case '+':
          result=num1+num2;
          num1=result;
          num2=-1;
          lcd.setCursor(0,0);
          lcd.print(result);
          break;
        case '-':
          result = num1 - num2;
          num1=result;
          num2=-1;
          lcd.setCursor(0,0);
          lcd.print(result);
          
          break;
         case '/':
          result = num1 / num2;
          num1=result;
          num2=-1;
          lcd.setCursor(0,0);
          lcd.print(result);
          break;
         case '*':
          result = num1 * num2;
          num1=result;
          num2=-1;
          lcd.setCursor(0,0);
          lcd.print(result);
          break;
         case '=':
          lcd.setCursor(0,0);
          lcd.print(result);
          num1=result;
        
      }
    }
  }
*/
  
}
