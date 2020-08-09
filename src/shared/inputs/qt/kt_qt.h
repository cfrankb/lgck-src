class QString;

class CKeyTranslator
{

public:
    CKeyTranslator();
    ~CKeyTranslator();
    static int translate(int key);
    static int translateLgck2Text(int key, QString &text);
    static int translateText2Lgck(const QString text);

protected:

};

