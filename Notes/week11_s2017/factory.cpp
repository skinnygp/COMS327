class object {
  object(const object_description &o) {
    ...
  }
  //static method can be called without an instance of the class
  //must be implemented outside of the class definition
  static object *create_object(const vector<object_description> &o);
};

class object_description {
  int weight;
  int color;
  dice damage;
  int speed;
  ...

  object *create_object() {
    return new object(weight, color, damage...);
  }

};

object *object::create_object(const vector<object_description> &o)
{
  const object_description &ob = o[rand() % o.length()];
  return new object(ob.weight, ob.color...);
}
