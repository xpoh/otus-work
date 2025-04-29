import sys
import json

def total_price(quantity, price):
    return quantity * price

if __name__ == "__main__":
    data = json.load(sys.stdin)
    quantity = data['quantity']
    price = data['price']
    print(total_price(quantity, price))
