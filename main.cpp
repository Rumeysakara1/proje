#include <QApplication>
#include <QMainWindow>
#include <QTabWidget>
#include <QTableWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QMessageBox>
#include <QGroupBox>
#include <QHeaderView>
#include <QRandomGenerator>
#include <QDateEdit>
#include <QComboBox>
#include <QDialog>
#include <QDebug>
#include <QStackedWidget>
#include <QPixmap>
#include <QString> // QString sınıfı için
#include <QDoubleSpinBox> // Yeni: Fiyat girişi için
#include <QFormLayout> // Yeni: Form düzeni için
#include <QTimer>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDir>
#include <iostream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <stdexcept>
#include <QFileDialog> // Dosya seçme penceresi için gerekli
#include <QInputDialog>
using namespace std;

/* ===================== SINIFLAR VE KALITIM HİYERARŞİLERİ ===================== */
class Customer;
class Vehicle;
Customer* findCustomerByUsername(const string& username);
Vehicle* findVehicle(const string& plate);
class Branch;

class NotificationSystem {
public:
    void sendNotification(const string& username, const string& message) {
        cout << "NOTIFICATION sent to " << username << ": " << message << endl;
    }
};

class Employee { // ABSTRACT
protected:
    string username;
    string password;
    string name;
public:
    Employee(string u, string p, string n) : username(u), password(p), name(n) {}
    virtual ~Employee() {}
    virtual string getRole() = 0;
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getName() const { return name; }
};

class Manager : public Employee {
public:
    Manager(string u, string p, string n) : Employee(u, p, n) {}
    string getRole() override { return "Manager"; }
};

class Sales : public Employee {
public:
    Sales(string u, string p, string n) : Employee(u, p, n) {}
    string getRole() override { return "Sales"; }
};

class Customer {
public:
    string name, phone;
    int id;
    string username;
    string password;
    int totalRentals;

    Customer(string n, string p, int i, string u, string pass)
        : name(n), phone(p), id(i), username(u), password(pass), totalRentals(0) {}
    string getUsername() const { return username; }
    string getPassword() const { return password; }
    string getName() const { return name; }
};


class Branch {
public:
    int id;
    string name;
    string address;
    Manager* manager;

    Branch(int i, string n, string a, Manager* m) : id(i), name(n), address(a), manager(m) {}
};


class Vehicle { // ABSTRACT
protected:
    string plate, model, brand;
    double pricePerDay;
    string status;
    Branch* branch;
    string imagePath;
public:
    // Yapıcı (Constructor)
    void setImagePath(string path) { imagePath = path; }
    string getImagePath() const { return imagePath; }

    Vehicle(string p, string m, string b, double pr, Branch* br)
        : plate(p), model(m), brand(b), pricePerDay(pr), status("Available"), branch(br) {}

    // Sanal Yıkıcı (Virtual Destructor)
    virtual ~Vehicle() {}

    // Saf Sanal Fonksiyon (Pure Virtual Function)
    virtual double calculatePrice(int d) = 0;
    virtual string getType() = 0;

    // --- Getter Fonksiyonları ---
    string getPlate() const { return plate; }
    string getModel() const { return model; }
    string getBrand() const { return brand; }
    double getPricePerDay() const { return pricePerDay; }
    string getStatus() const { return status; }

    // --- Setter Fonksiyonları ---

    // Fiyatı ayarlayan setter (Önceki hata çözümü için kritik)
    void setPricePerDay(double newPrice) {
        pricePerDay = newPrice;
    }

    // Durumu ayarlayan setter (Mevcut kodunuzda da kullanılıyor)
    void setStatus(string s) {
        status = s;
    }
};

class EconomyCar : public Vehicle {
public:
    EconomyCar(string p, string m, string b, Branch* br) : Vehicle(p, m, b, 500.0, br) {}
    double calculatePrice(int d) override { return d * pricePerDay; }
    string getType() override { return "Economy"; }
};

class LuxuryCar : public Vehicle {
public:
    LuxuryCar(string p, string m, string b, Branch* br) : Vehicle(p, m, b, 1200.0, br) {}
    double calculatePrice(int d) override { return d * pricePerDay; }
    string getType() override { return "Luxury"; }
};


class Insurance { // ABSTRACT
protected:
    double dailyCost;
public:
    Insurance(double cost) : dailyCost(cost) {}
    virtual ~Insurance() {}
    virtual string getName() = 0;
    double getDailyCost() const { return dailyCost; }
};

class BasicInsurance : public Insurance {
public:
    BasicInsurance() : Insurance(50.0) {}
    string getName() override { return "Basic"; }
};

class PremiumInsurance : public Insurance {
public:
    PremiumInsurance() : Insurance(150.0) {}
    string getName() override { return "Premium"; }
};


class Rental {
public:
    int id;
    Customer* customer;
    Vehicle* vehicle;
    int days;
    double price;
    string startDate;
    string endDate;
    string status;
    Insurance* insurance;

    Rental(int rentalId, Customer* c, Vehicle* v, int d, string sDate, Insurance* ins) {
        id = rentalId;
        customer = c;
        vehicle = v;
        days = d;
        insurance = ins;
        price = v->calculatePrice(d) + (ins->getDailyCost() * d);
        startDate = sDate;
        endDate = "N/A";
        status = "Active";
        c->totalRentals++;
    }
};

class MaintenanceRecord {
public:
    int id;
    Vehicle* vehicle;
    string type;
    string description;
    double cost;
    string date;

    MaintenanceRecord(int mid, Vehicle* v, string t, string desc, double c, string d)
        : id(mid), vehicle(v), type(t), description(desc), cost(c), date(d) {
        v->setStatus("Maintenance");
    }
};

class Payment {
protected:
    double amount;
    string date;
    int rentalId; // YENİ: Rental ID alanı
public:
    // Yapıcıyı 3 parametre alacak şekilde güncelleyin
    Payment(double a, string d, int rId) : amount(a), date(d), rentalId(rId) {}

    virtual ~Payment() {}

    // Alt sınıfların uygulamasını zorunlu kılmak için saf sanal (pure virtual)
    virtual string getType() = 0;

    // Getter'lar
    int getRentalId() const { return rentalId; }
    double getAmount() const { return amount; } // getAmount eklendi (image_01cd72.png hatasını çözer)
    string getDate() const { return date; }     // getDate eklendi (image_01cd72.png hatasını çözer)
};

class CashPayment : public Payment {
public:
    // Yapıcıyı 3 parametre alacak şekilde güncelleyin (amount, date, rentalId)
    CashPayment(double a, string d, int rId)
        : Payment(a, d, rId) {}

    string getType() override { return "Cash"; } // Doğru override
};

class CardPayment : public Payment {
private:
    string cardNumber;
public:
    // Yapıcıyı 4 parametre alacak şekilde güncelleyin (amount, date, cardNumber, rentalId)
    CardPayment(double a, string d, string cn, int rId)
        : Payment(a, d, rId), cardNumber(cn) {}

    string getType() override { return "Card"; } // Doğru override
};

/*class OnlinePayment : public Payment {
public:
    // Online ödeme olduğu için kart numarası zorunlu değil, 3 parametre yeterli.
    OnlinePayment(double a, string d, int rId)
        : Payment(a, d, rId) {}

    string getType() override { return "Online Transfer"; }
};
*/

/* ===================== GLOBAL VERİ VE YARDIMCI FONKSİYONLAR ===================== */

vector<Employee*> employees;
vector<Branch*> branches;
vector<Insurance*> insuranceOptions;
vector<Customer*> customers;
vector<Vehicle*> vehicles;
vector<Rental*> rentals;
vector<Payment*> payments;
vector<MaintenanceRecord*> maintenanceRecords;

NotificationSystem notificationSystem;

Employee* findEmployeeByUsername(const std::string& username);
Vehicle* findVehicle(const string& plate);

// --- JSON'a Kaydetme Fonksiyonu ---
void saveCustomersToJson() {
    QJsonArray customerArray;

    for (Customer* c : customers) {
        QJsonObject customerObject;
        customerObject["name"] = QString::fromStdString(c->name);
        customerObject["phone"] = QString::fromStdString(c->phone);
        customerObject["id"] = c->id;
        customerObject["username"] = QString::fromStdString(c->username);
        customerObject["password"] = QString::fromStdString(c->password);
        customerObject["totalRentals"] = c->totalRentals;
        customerArray.append(customerObject);
    }

    QJsonDocument doc(customerArray);
    QFile file("customers.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(doc.toJson());
        file.close();
        qDebug() << "Kullanıcılar JSON dosyasına kaydedildi.";
    }
}

// --- ARAÇLARI KAYDETME VE YÜKLEME ---
void saveVehiclesToJson() {
    QJsonArray vehicleArray;
    for (Vehicle* v : vehicles) {
        QJsonObject obj;
        obj["plate"] = QString::fromStdString(v->getPlate());
        obj["brand"] = QString::fromStdString(v->getBrand());
        obj["model"] = QString::fromStdString(v->getModel());
        obj["type"] = QString::fromStdString(v->getType());
        obj["price"] = v->getPricePerDay();
        obj["status"] = QString::fromStdString(v->getStatus());
        obj["imagePath"] = QString::fromStdString(v->getImagePath());
        // Şube ID'si (Varsayılan olarak ilk şube veya aracın şubesi)
        obj["branchId"] = (branches.empty()) ? 1 : branches[0]->id;

        vehicleArray.append(obj);
    }

    QFile file("vehicles.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(vehicleArray).toJson());
        file.close();
    }
}

void loadVehiclesFromJson() {
    QFile file("vehicles.json");
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonArray array = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    vehicles.clear(); // Mevcut listeyi temizle

    Branch* defaultBranch = branches.empty() ? nullptr : branches[0];

    for (const QJsonValue &value : array) {
        QJsonObject obj = value.toObject();
        string type = obj["type"].toString().toStdString();
        string plate = obj["plate"].toString().toStdString();
        string model = obj["model"].toString().toStdString();
        string brand = obj["brand"].toString().toStdString();
        double price = obj["price"].toDouble();

        Vehicle* v = nullptr;
        if (type == "Economy") v = new EconomyCar(plate, model, brand, defaultBranch);
        else v = new LuxuryCar(plate, model, brand, defaultBranch);

        if (v) {
            v->setPricePerDay(price);
            v->setStatus(obj["status"].toString().toStdString());
            v->setImagePath(obj["imagePath"].toString().toStdString());
            vehicles.push_back(v);
        }
    }
}

// --- KİRALAMALARI (RENTALS) KAYDETME VE YÜKLEME ---
void saveRentalsToJson() {
    QJsonArray rentalArray;
    for (Rental* r : rentals) {
        QJsonObject obj;
        obj["id"] = r->id;
        obj["customerUser"] = QString::fromStdString(r->customer->getUsername());
        obj["vehiclePlate"] = QString::fromStdString(r->vehicle->getPlate());
        obj["days"] = r->days;
        obj["startDate"] = QString::fromStdString(r->startDate);
        obj["endDate"] = QString::fromStdString(r->endDate);
        obj["status"] = QString::fromStdString(r->status);
        obj["insuranceName"] = QString::fromStdString(r->insurance->getName());
        obj["price"] = r->price;

        rentalArray.append(obj);
    }

    QFile file("rentals.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(rentalArray).toJson());
        file.close();
    }
}

void loadRentalsFromJson() {
    QFile file("rentals.json");
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonArray array = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    rentals.clear();

    for (const QJsonValue &value : array) {
        QJsonObject obj = value.toObject();

        // İlişkili nesneleri bul
        Customer* c = findCustomerByUsername(obj["customerUser"].toString().toStdString());
        Vehicle* v = findVehicle(obj["vehiclePlate"].toString().toStdString());

        // Sigortayı bul (Basitçe yeniden oluşturuyoruz veya listeden seçiyoruz)
        Insurance* ins = nullptr;
        string insName = obj["insuranceName"].toString().toStdString();
        if (insName == "Premium") ins = new PremiumInsurance();
        else ins = new BasicInsurance(); // Varsayılan

        if (c && v) {
            Rental* r = new Rental(
                obj["id"].toInt(), c, v,
                obj["days"].toInt(),
                obj["startDate"].toString().toStdString(),
                ins
                );
            r->endDate = obj["endDate"].toString().toStdString();
            r->status = obj["status"].toString().toStdString();
            r->price = obj["price"].toDouble();

            rentals.push_back(r);
        }
    }
}





// --- JSON'dan Yükleme Fonksiyonu ---
void loadCustomersFromJson() {
    QFile file("customers.json");
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray customerArray = doc.array();

    // Mevcut listeyi temizle (çakışma olmaması için)
    customers.clear();

    for (int i = 0; i < customerArray.size(); ++i) {
        QJsonObject obj = customerArray[i].toObject();
        Customer* c = new Customer(
            obj["name"].toString().toStdString(),
            obj["phone"].toString().toStdString(),
            obj["id"].toInt(),
            obj["username"].toString().toStdString(),
            obj["password"].toString().toStdString()
            );
        c->totalRentals = obj["totalRentals"].toInt();
        customers.push_back(c);
    }
    qDebug() << "Kullanıcılar JSON'dan yüklendi.";
}

void saveEmployeesToJson() {
    QJsonArray employeeArray;
    for (Employee* e : employees) {
        QJsonObject obj;
        obj["username"] = QString::fromStdString(e->getUsername());
        obj["password"] = QString::fromStdString(e->getPassword());
        obj["name"] = QString::fromStdString(e->getName());
        obj["role"] = QString::fromStdString(e->getRole()); // Manager mı Sales mi?
        employeeArray.append(obj);
    }
    QFile file("employees.json");
    if (file.open(QIODevice::WriteOnly)) {
        file.write(QJsonDocument(employeeArray).toJson());
        file.close();
    }
}

void loadEmployeesFromJson() {
    QFile file("employees.json");
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonArray array = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    if(!array.isEmpty()) employees.clear();

    for (int i = 0; i < array.size(); ++i) {
        QJsonObject obj = array[i].toObject();
        string role = obj["role"].toString().toStdString();

        if (role == "Manager") {
            employees.push_back(new Manager(obj["username"].toString().toStdString(),
                                            obj["password"].toString().toStdString(),
                                            obj["name"].toString().toStdString()));
        } else {
            employees.push_back(new Sales(obj["username"].toString().toStdString(),
                                          obj["password"].toString().toStdString(),
                                          obj["name"].toString().toStdString()));

        }}}

Rental* findRentalById(int id) {
    auto it = find_if(rentals.begin(), rentals.end(),
                      [&](Rental* r){ return r->id == id; });
    return (it != rentals.end()) ? *it : nullptr;
}

// YENİ YARDIMCI FONKSİYON: Şube Ekleme Diyaloğu

// --- Şube Ekleme Fonksiyonu (Hatalar Giderildi) ---
void createAddBranchDialog(const std::function<void()>& updateFunc) {
    // Statik yerine dinamik (new) kullanarak pointer yönetimi yapıyoruz
    QDialog *dialog = new QDialog();
    dialog->setWindowTitle("Yeni Şube Ekle");
    dialog->setFixedSize(400, 400);
    dialog->setAttribute(Qt::WA_DeleteOnClose); // Bellek yönetimi için önemli

    QVBoxLayout *mainLayout = new QVBoxLayout(dialog);
    QFormLayout *formLayout = new QFormLayout();

    QLineEdit *nameInput = new QLineEdit();
    QLineEdit *addressInput = new QLineEdit();
    QLineEdit *phoneInput = new QLineEdit();
    QComboBox *managerCombo = new QComboBox();

    // Manager listesini doldur
    for (Employee* e : employees) {
        if (e->getRole() == "Manager") {
            managerCombo->addItem(QString::fromStdString(e->getName() + " (" + e->getUsername() + ")"));
        }
    }

    formLayout->addRow("Şube Adı:", nameInput);
    formLayout->addRow("Adres:", addressInput);
    formLayout->addRow("Telefon:", phoneInput);
    formLayout->addRow("Yönetici Seçin:", managerCombo);
    mainLayout->addLayout(formLayout);

    // Butonlar
    QHBoxLayout *btnLayout = new QHBoxLayout();
    QPushButton *saveBtn = new QPushButton("Record Branch");
    QPushButton *cancelBtn = new QPushButton("Cancel");

    saveBtn->setStyleSheet("background-color: #007BFF; color: white; height: 30px;");
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);
    mainLayout->addLayout(btnLayout);

    // Sinyal Bağlantıları
    QObject::connect(cancelBtn, &QPushButton::clicked, dialog, &QDialog::reject);

    QObject::connect(saveBtn, &QPushButton::clicked, [=]() {
        QString name = nameInput->text();
        QString address = addressInput->text();
        QString phone = phoneInput->text();

        if (name.isEmpty() || address.isEmpty()) {
            QMessageBox::warning(dialog, "Error", "Branch name and address cannot be empty.");
            return;
        }

        // Username ayıklama mantığı: "Ali Veli (aliveli)" -> "aliveli"
        QString selectedText = managerCombo->currentText();
        QString managerUsername = selectedText.section('(', -1).section(')', 0, 0);

        Employee* manager = findEmployeeByUsername(managerUsername.toStdString());

        if (!manager) {
            QMessageBox::critical(dialog, "Error", "Yönetici bulunamadı.");
            return;
        }

        // Yeni Şube Oluşturma
        int newBranchId = branches.empty() ? 101 : branches.back()->id + 1;
        Branch* newBranch = new Branch(newBranchId, name.toStdString(), address.toStdString(), static_cast<Manager*>(manager));
        branches.push_back(newBranch);

        QMessageBox::information(dialog, "Başarılı", name + " şubesi eklendi.");

        updateFunc(); // Tabloları güncelle
        dialog->accept();
    });

    dialog->exec();
}

// GÜNCELLENDİ: Araç modeline göre resim dosya yolunu döndürür (Tüm uzantılar artık .jpg)
QString getVehicleImagePath (const string& model)  {   // Butonlar
    std::string m = model;
    // Küçük harfe çevirerek eşleşme kolaylığı sağlarız
    std::transform(m.begin(), m.end(), m.begin(), ::tolower);

    if (m.find("clio") != std::string::npos) return ":/images/clio.jpg";
    if (m.find("520i") != std::string::npos) return ":/images/520i.jpg";
    if (m.find("egea") != std::string::npos) return ":/images/egea.jpg";
    if (m == "welcome") return ":/images/welcome.jpg";




    return ":/images/default_car.jpg"; // Varsayılan resim
}


Vehicle* findVehicle(const string& plate){
    auto it = find_if(vehicles.begin(), vehicles.end(),
                      [&](Vehicle* v){ return v->getPlate() == plate; });
    return (it != vehicles.end()) ? *it : nullptr;
}

Customer* findCustomerByUsername(const string& username) {
    auto it = find_if(customers.begin(), customers.end(),
                      [&](Customer* c){ return c->username == username; });
    return (it != customers.end()) ? *it : nullptr;
}

Employee* findEmployeeByUsername(const string& username) {
    auto it = find_if(employees.begin(), employees.end(),
                      [&](Employee* e){ return e->getUsername() == username; });
    return (it != employees.end()) ? *it : nullptr;
}

// --- Müşteri Paneli Tablosunu Güncelleyen Fonksiyon (Resim Destekli) ---
void updateCustomerPanelTable(QTableWidget* table) {
    if (!table) return;

    // Tabloyu sıfırla ve başlıkları ayarla
    table->setRowCount(0);
    table->setColumnCount(5);
    table->setHorizontalHeaderLabels({"Görsel", "Model", "Tip", "Günlük Fiyat", "Plaka"});
    table->setColumnWidth(0, 100); // Resim sütunu genişliği

    // Sütunların pencereye yayılmasını sağla (Resim hariç)
    table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
    table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed); // Resim sabit kalsın

    for (Vehicle* v : vehicles) {
        // Sadece Bakımda OLMAYAN araçları göster (Reserved olanlar da listede kalır)
        if (v->getStatus() != "Maintenance") {
            int row = table->rowCount();
            table->insertRow(row);
            table->setRowHeight(row, 80); // Resim için satır yüksekliği

            // --- RESİM YÜKLEME MANTIĞI (BAŞLANGIÇ) ---
            QLabel *imageLabel = new QLabel();
            QPixmap pixmap;
            bool imageLoaded = false;

            // 1. ADIM: Manager tarafından yüklenen özel resim var mı?
            // (Vehicle sınıfına eklediğimiz getImagePath fonksiyonunu kullanıyoruz)
            QString customPath = QString::fromStdString(v->getImagePath());

            if (!customPath.isEmpty()) {
                if (pixmap.load(customPath)) {
                    imageLoaded = true;
                }
            }

            // 2. ADIM: Özel resim yoksa, varsayılan kaynaklardan (resources) yükle
            if (!imageLoaded) {
                QString modelName = QString::fromStdString(v->getModel()).trimmed();
                QStringList tryPaths;
                // Denenecek dosya yolları (Sırasıyla)
                tryPaths << ":/images/" + modelName.toLower() + ".jpg"; // örn: clio.jpg
                tryPaths << ":/images/" + modelName + ".jpg";          // örn: Clio.jpg
                tryPaths << ":/images/" + modelName.toUpper() + ".jpg"; // örn: CLIO.jpg
                tryPaths << ":/images/default_car.jpg";                // Varsayılan resim

                for (const QString& path : tryPaths) {
                    if (pixmap.load(path)) {
                        imageLoaded = true;
                        break;
                    }
                }
            }

            // Resmi Etikete Ata
            if (imageLoaded) {
                imageLabel->setPixmap(pixmap.scaled(100, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                imageLabel->setText("Görsel Yok");
                imageLabel->setStyleSheet("color: gray; font-size: 10px;");
            }

            imageLabel->setAlignment(Qt::AlignCenter);
            table->setCellWidget(row, 0, imageLabel);
            // --- RESİM YÜKLEME MANTIĞI (BİTİŞ) ---

            // Diğer Sütun Bilgileri
            table->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(v->getModel())));
            table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(v->getType())));
            table->setItem(row, 3, new QTableWidgetItem(QString::number(v->getPricePerDay(), 'f', 2) + " TL"));
            table->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(v->getPlate())));
        }
    }
}

// Yeni araç ekleme diyalogunu oluşturan yardımcı fonksiyon
void createAddVehicleDialog(QTableWidget* vehicleTable, const std::vector<Branch*>& branches,
                            const std::function<void()>& updateVehicleTable, NotificationSystem& notificationSystem) {
    QDialog dialog;
    dialog.setWindowTitle("Yeni Araç Ekle");
    dialog.setFixedSize(400, 450); // Yükseklik biraz arttı

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    // Form Elemanları
    QLineEdit *plateInput = new QLineEdit();
    QLineEdit *brandInput = new QLineEdit();
    QLineEdit *modelInput = new QLineEdit();
    QDoubleSpinBox *priceInput = new QDoubleSpinBox();
    priceInput->setRange(100.0, 5000.0);
    priceInput->setValue(600.0);
    QComboBox *typeCombo = new QComboBox();
    typeCombo->addItem("Economy");
    typeCombo->addItem("Luxury");

    // --- YENİ: Resim Seçme Bölümü ---
    QLabel *imagePathLabel = new QLabel("Görsel seçilmedi");
    imagePathLabel->setStyleSheet("color: gray; font-style: italic;");
    QPushButton *selectImageBtn = new QPushButton("Araç Görseli Yükle");
    QString *selectedFilePath = new QString(""); // Seçilen dosya yolunu tutmak için

    QObject::connect(selectImageBtn, &QPushButton::clicked, [&]() {

        // Dosya seçme penceresini aç (Sadece resim dosyaları)
        // &dialog artık hata vermez çünkü orijinal nesneye erişiyoruz.
        QString fileName = QFileDialog::getOpenFileName(&dialog, "Resim Seç", "", "Images (*.png *.jpg *.jpeg)");

        if (!fileName.isEmpty()) {
            *selectedFilePath = fileName;
            imagePathLabel->setText("Seçilen: " + fileName.section('/', -1));
            imagePathLabel->setStyleSheet("color: green; font-weight: bold;");
        }
    });


    // --------------------------------

    QGroupBox *formGroup = new QGroupBox("Araç Bilgileri");
    QFormLayout *formLayout = new QFormLayout(formGroup);
    formLayout->addRow("Plaka:", plateInput);
    formLayout->addRow("Marka:", brandInput);
    formLayout->addRow("Model:", modelInput);
    formLayout->addRow("Günlük Fiyat:", priceInput);
    formLayout->addRow("Tip:", typeCombo);
    formLayout->addRow("Görsel:", selectImageBtn); // Butonu forma ekle
    formLayout->addRow("", imagePathLabel);        // Etiketi altına ekle

    QPushButton *saveBtn = new QPushButton("Aracı Kaydet");
    saveBtn->setStyleSheet("background-color: #28a745; color: white;");

    mainLayout->addWidget(formGroup);
    mainLayout->addWidget(saveBtn);

    QObject::connect(saveBtn, &QPushButton::clicked, [&, selectedFilePath]() {
        QString plate = plateInput->text().toUpper();
        QString brand = brandInput->text();
        QString model = modelInput->text();
        double price = priceInput->value();
        QString type = typeCombo->currentText();

        if (plate.isEmpty() || brand.isEmpty() || model.isEmpty()) {
            QMessageBox::warning(&dialog, "Hata", "Lütfen gerekli alanları doldurun.");
            return;
        }

        // Araç Nesnesini Oluştur
        Vehicle* newVehicle = nullptr;
        Branch* defaultBranch = branches.empty() ? nullptr : branches[0];

        if (type == "Economy") newVehicle = new EconomyCar(plate.toStdString(), model.toStdString(), brand.toStdString(), defaultBranch);
        else newVehicle = new LuxuryCar(plate.toStdString(), model.toStdString(), brand.toStdString(), defaultBranch);

        if (newVehicle) {
            newVehicle->setPricePerDay(price);

            // --- YENİ: Resmi Kopyalama ve Kaydetme Mantığı ---
            if (!selectedFilePath->isEmpty()) {
                // 1. "vehicle_images" klasörü yoksa oluştur
                QDir dir;
                if (!dir.exists("vehicle_images")) {
                    dir.mkpath("vehicle_images");
                }

                // 2. Resmi yeni isimle (Plaka.jpg) kopyala
                QString targetPath = "vehicle_images/" + plate + ".jpg";

                // Eğer eski bir dosya varsa sil
                if (QFile::exists(targetPath)) QFile::remove(targetPath);

                // Kopyalama işlemi
                if (QFile::copy(*selectedFilePath, targetPath)) {
                    // Başarılıysa nesneye kaydet
                    newVehicle->setImagePath(targetPath.toStdString());
                } else {
                    qDebug() << "Resim kopyalanamadı!";
                }
            }
            // ------------------------------------------------

            vehicles.push_back(newVehicle);

            saveVehiclesToJson();

            updateVehicleTable();
            notificationSystem.sendNotification("Manager", "New vehicle added.");
            QMessageBox::information(&dialog, "Başarılı", "Araç eklendi.");
            dialog.accept();
        }
    });

    dialog.exec();
}



void createAddMaintenanceDialog(QTableWidget* maintenanceTable, const std::vector<Vehicle*>& vehicles,
                                const std::function<void()>& updateMaintenanceTable,
                                const std::function<void()>& updateVehicleTable,
                                const std::function<void()>& updateAllTablesFunc,// KRİTİK: Bu parametreyi ekle
                                NotificationSystem& notificationSystem) {
    QDialog dialog;
    dialog.setWindowTitle("Yeni Bakım Kaydı Ekle");
    dialog.setFixedSize(450, 350);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    // Form Elemanları
    QComboBox *vehicleCombo = new QComboBox();
    for (Vehicle* v : vehicles) {
        if (v->getStatus() != "Maintenance") { // Bakımda olmayanları listele
            vehicleCombo->addItem(QString::fromStdString(v->getPlate()) + " - " + QString::fromStdString(v->getModel()));
        }
    }

    // Eğer araç yoksa uyarı ver
    if (vehicleCombo->count() == 0) {
        QMessageBox::information(&dialog, "Bilgi", "Bakıma alınabilecek uygun araç bulunmamaktadır.");
        dialog.reject();
        return;
    }

    QLineEdit *typeInput = new QLineEdit();
    typeInput->setPlaceholderText("Örn: Yağ Değişimi, Lastik Değişimi");
    QLineEdit *descriptionInput = new QLineEdit();
    descriptionInput->setPlaceholderText("Açıklama");
    QDoubleSpinBox *costInput = new QDoubleSpinBox();
    costInput->setRange(0.0, 50000.0);
    costInput->setSuffix(" TL");
    QDateEdit *dateEdit = new QDateEdit(QDate::currentDate());
    dateEdit->setDisplayFormat("dd/MM/yyyy");

    // Layout'a Ekleme
    QGroupBox *formGroup = new QGroupBox("Bakım Bilgileri");
    QFormLayout *formLayout = new QFormLayout(formGroup);
    formLayout->addRow("Araç (Plaka - Model):", vehicleCombo);
    formLayout->addRow("Bakım Tipi:", typeInput);
    formLayout->addRow("Açıklama:", descriptionInput);
    formLayout->addRow("Maliyet:", costInput);
    formLayout->addRow("Tarih:", dateEdit);

    // Butonlar
    QPushButton *saveBtn = new QPushButton("Kaydı Oluştur");
    QPushButton *cancelBtn = new QPushButton("İptal");
    saveBtn->setStyleSheet("background-color: #A9A9A9; color: black;");

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addWidget(saveBtn);
    btnLayout->addWidget(cancelBtn);

    mainLayout->addWidget(formGroup);
    mainLayout->addLayout(btnLayout);


    // SİNYAL BAĞLANTILARI
    QObject::connect(cancelBtn, &QPushButton::clicked, &dialog, &QDialog::reject);
    QObject::connect(saveBtn, &QPushButton::clicked, [&, updateAllTablesFunc]() {
        QString selectedVehicleText = vehicleCombo->currentText();
        QString plate = selectedVehicleText.split(" - ").at(0); // Plakayı al
        QString type = typeInput->text();
        QString description = descriptionInput->text();
        double cost = costInput->value();
        string date = dateEdit->date().toString("dd/MM/yyyy").toStdString();

        if (type.isEmpty() || description.isEmpty()) {
            QMessageBox::warning(&dialog, "Hata", "Bakım Tipi ve Açıklama alanları boş bırakılamaz.");
            return;
        }

        Vehicle* vehicleToMaintain = findVehicle(plate.toStdString());

        if (!vehicleToMaintain) {
            QMessageBox::critical(&dialog, "Hata", "Seçilen araç sistemde bulunamadı.");
            return;
        }

        // Bakım Kaydı Nesnesini Oluşturma
        int newMaintenanceId = maintenanceRecords.empty() ? 1 : maintenanceRecords.back()->id + 1;

        // ÖNEMLİ: MaintenanceRecord yapıcı içinde aracın durumu otomatik olarak "Maintenance" olur.
        MaintenanceRecord* newRecord = new MaintenanceRecord(
            newMaintenanceId,
            vehicleToMaintain,
            type.toStdString(),
            description.toStdString(),
            cost,
            date
            );

        maintenanceRecords.push_back(newRecord);
        updateMaintenanceTable(); // Bakım tablosunu güncelle
        updateVehicleTable();     // Araç durumları değiştiği için araç tablosunu da güncelle

        notificationSystem.sendNotification(
            "Manager",
            "Vehicle " + plate.toStdString() + " is now in maintenance (" + type.toStdString() + ")."
            );

        QMessageBox::information(&dialog, "Başarılı", QString("'%1' plakalı araç için bakım kaydı başarıyla oluşturuldu.").arg(plate));
        updateAllTablesFunc(); // <<< YENİ ÇAĞRI
        dialog.accept();
    });

    dialog.exec();
}


// --- Kredi Kartı Ödemesi İçin Yardımcı Fonksiyon ---
Payment* createCreditCardPayment(double amount, std::string date, int rentalId, QWidget* parent) {
    QDialog cardDialog(parent);
    cardDialog.setWindowTitle("Kredi Kartı Bilgileri");
    cardDialog.setFixedSize(350, 250);

    QFormLayout *layout = new QFormLayout(&cardDialog);

    QLineEdit *cardHolder = new QLineEdit();
    QLineEdit *cardNumber = new QLineEdit();
    cardNumber->setInputMask("9999 9999 9999 9999"); // Formatı zorunlu kılar

    QLineEdit *expiry = new QLineEdit();
    expiry->setInputMask("99/99");

    QLineEdit *cvv = new QLineEdit();
    cvv->setEchoMode(QLineEdit::Password);
    cvv->setMaxLength(3);

    layout->addRow("Kart Sahibi:", cardHolder);
    layout->addRow("Kart Numarası:", cardNumber);
    layout->addRow("Son Kullanma:", expiry);
    layout->addRow("CVV:", cvv);

    QPushButton *payBtn = new QPushButton("Ödemeyi Onayla");
    layout->addRow(payBtn);

    Payment* newPayment = nullptr;

    // Butona tıklandığında nesneyi oluştur ve diyaloğu kapat
    QObject::connect(payBtn, &QPushButton::clicked, [&]() {
        if (cardHolder->text().isEmpty() || cardNumber->text().length() < 19) {
            QMessageBox::warning(&cardDialog, "Hata", "Lütfen bilgileri eksiksiz girin.");
            return;
        }

        // Ödeme nesnesini burada oluşturuyoruz
        newPayment = new CardPayment(amount, date, cardNumber->text().right(4).toStdString(), rentalId);
        QMessageBox::information(&cardDialog, "Başarılı", "Tahsilat işlemi onaylandı.");


        cardDialog.accept(); // exec() sonucunu QDialog::Accepted yapar
    });

    // Diyalog kapanana kadar burada bekler (Synchronous)
    if (cardDialog.exec() == QDialog::Accepted) {
        return newPayment;
    }

    if (newPayment) delete newPayment;
    return nullptr; // İptal edilirse boş döner
}



// --- YENİ EKLENECEK FONKSİYON: Tarih Çakışma Kontrolü ---
bool isVehicleAvailableForDates(Vehicle* vehicle, QDate newStartDate, int daysToCheck) {
    // Yeni kiralamanın bitiş tarihini hesapla
    QDate newEndDate = newStartDate.addDays(daysToCheck);

    for (Rental* r : rentals) {
        // 1. Sadece aynı araca ait kayıtları kontrol et
        if (r->vehicle != vehicle) continue;

        // 2. İptal edilmiş veya tamamlanmış kiralamaları yoksay
        if (r->status == "Cancelled" || r->status == "Completed") continue;

        // 3. Mevcut kiralamanın tarihlerini QDate'e çevir
        QDate existingStartDate = QDate::fromString(QString::fromStdString(r->startDate), "dd/MM/yyyy");

        // Rental sınıfında "days" tutulduğu için bitiş tarihini hesaplıyoruz
        QDate existingEndDate = existingStartDate.addDays(r->days);

        // 4. ÇAKIŞMA MANTIĞI (Overlap Logic)
        // Eğer (Yeni Başlangıç <= Mevcut Bitiş) VE (Yeni Bitiş >= Mevcut Başlangıç) ise çakışma vardır.
        if (newStartDate <= existingEndDate && newEndDate >= existingStartDate) {
            return false; // Araç bu tarihlerde dolu!
        }
    }
    return true; // Hiçbir çakışma yok, kiralanabilir.
}





/* ===================== MAIN ===================== */

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    std::function<void()> updateRentalTable;

    loadCustomersFromJson();

    // Eğer JSON dosyası boşsa veya ilk kez çalışıyorsa örnek müşteri ekle
    if (customers.empty()) {
        customers.push_back(new Customer("Test Customer", "05335555555", 1001, "customer1", "123"));
        saveCustomersToJson();
    }
    loadEmployeesFromJson();
    if (employees.empty()) {
        // Eğer dosya yoksa Alice ve Bob'u varsayılan olarak oluştur
        employees.push_back(new Manager("manager", "123", " Manager"));
        employees.push_back(new Sales("sales", "123", " Sales"));
        saveEmployeesToJson(); // Dosyayı oluştur
    }

    loadVehiclesFromJson(); // Araçları yükle

    // Eğer hiç araç yoksa (ilk kurulum), varsayılanları ekle ve kaydet
    if (vehicles.empty()) {
        Branch* defaultBranch = branches.empty() ? nullptr : branches[0];

        if (defaultBranch) {
            vehicles.push_back(new EconomyCar("34ABC11", "Clio", "Renault", defaultBranch));
            vehicles.push_back(new LuxuryCar("06XYZ99", "520i", "BMW", defaultBranch));
            vehicles.push_back(new EconomyCar("16DEF22", "Egea", "Fiat", defaultBranch));
            saveVehiclesToJson();
        }

    }

    loadRentalsFromJson(); // Kiralamaları yükle (Araçlar ve Müşteriler yüklendikten SONRA)




    // Ana pencere ve yığılmış widget
    QMainWindow mainWindow;
    QStackedWidget *stackedWidget = new QStackedWidget;
    mainWindow.setCentralWidget(stackedWidget);
    mainWindow.setWindowTitle("🚗 Kiralama Yönetim Sistemi");
    mainWindow.resize(1100, 700);

    // --- BAŞLANGIÇ VERİSİ ---
    Manager* m1 = new Manager("manager", "123", "Manager");
    Sales* s1 = new Sales("sales", "123", "Bob Sales");
    employees.push_back(m1);
    employees.push_back(s1);

    //customers.push_back(new Customer("Test Müşteri", "5551112233", 1001, "customer1", "123"));

    Branch* b1 = new Branch(1, "Main Branch", "Ankara, Turkey", m1);
    branches.push_back(b1);

    insuranceOptions.push_back(new BasicInsurance());
    insuranceOptions.push_back(new PremiumInsurance());

    vehicles.push_back(new EconomyCar("34ABC11", "Clio", "Renault", b1));
    vehicles.push_back(new LuxuryCar("06XYZ99", "520i", "BMW", b1));
    vehicles.push_back(new EconomyCar("16DEF22", "Egea", "Fiat", b1));

    /* ===================== BİLEŞEN TANIMLARI ===================== */

    // ORTAK: Giriş/Kayıt Alanları
    QLineEdit *login_usernameInput = new QLineEdit();
    QLineEdit *login_passwordInput = new QLineEdit();
    login_passwordInput->setEchoMode(QLineEdit::Password);


    // ORTAK: Kayıt Alanları
    QLineEdit *register_nameInput = new QLineEdit();
    QLineEdit *register_phoneInput = new QLineEdit();
    QLineEdit *register_usernameInput = new QLineEdit();
    QLineEdit *register_passwordInput = new QLineEdit();
    register_passwordInput->setEchoMode(QLineEdit::Password);

    // YÖNETİM PANELİ
    QTabWidget *adminTabs = new QTabWidget();
    QTableWidget *vehicleTable = new QTableWidget(0, 7); // 7 sütun: Görsel + 6 veri
    QTableWidget *customerTable = new QTableWidget(0, 5);
    QTableWidget *rentalTable = new QTableWidget(0, 7);
    QTableWidget *paymentTable = new QTableWidget(0, 4);
    QTableWidget *maintenanceTable = new QTableWidget(0, 5);
    QPushButton *addVehicleBtn = new QPushButton("+ Add Vehicle");
    QPushButton *recordPaymentBtn = new QPushButton("Record New Payment");
    QPushButton *addMaintenanceBtn = new QPushButton("Add Maintenance Record");
    QPushButton *addBranchBtn = new QPushButton("Add New Branch");
    QTableWidget *branchTable = new QTableWidget();
    QTableWidget *employeeTable = new QTableWidget(0, 5); // <<< BU SATIRI EKLEYİN


    //********
    // Branch Tab (Layout)
    // Branch Tab (Layout)
    QWidget *branchesTab = new QWidget();
    QVBoxLayout *branchesLayout = new QVBoxLayout(branchesTab);

    QLabel *branchInfoLabel = new QLabel("<h3>🏢 Mevcut Şubeler</h3>");
    branchesLayout->addWidget(branchInfoLabel);

    // Tabloyu ekle
    branchTable->setColumnCount(4);
    branchTable->setHorizontalHeaderLabels({"ID", "Şube Adı", "Adres", "Müdür (Kullanıcı Adı)"});
    branchTable->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch); // Şube Adı genişlesin

    branchesLayout->addWidget(branchTable);
    branchesLayout->addWidget(addBranchBtn);
    //********

    // MÜŞTERİ PANELİ
    QTableWidget *customerVehicleTable = new QTableWidget(0, 5); // 5 sütun
    QLineEdit *rentDaysInput = new QLineEdit();
    QDateEdit *startDateEdit = new QDateEdit(QDate::currentDate());
    QComboBox *insuranceCombo = new QComboBox();
    QPushButton *rentCarBtn = new QPushButton("Seçili Aracı Kirala");


    // --- Tablo Güncelleme Lambda Fonksiyonları ---

    // --- GÜNCEL UPDATE VEHICLE TABLE (Resim Yükleme Özellikli) ---
    auto updateVehicleTable = [&]() {
        vehicleTable->setRowCount(0);
        vehicleTable->setColumnWidth(0, 100); // Görsel sütun genişliği
        vehicleTable->setHorizontalHeaderLabels({"Görsel", "Brand", "Model", "Type", "Plate", "Daily Rate", "Status"});

        for(Vehicle* v : vehicles) {
            int row = vehicleTable->rowCount();
            vehicleTable->insertRow(row);
            vehicleTable->setRowHeight(row, 80);

            // --- 1. RESİM YÜKLEME MANTIĞI (BAŞLANGIÇ) ---
            QLabel *imageLabel = new QLabel();
            QPixmap pixmap;

            // A) Önce Manager tarafından yüklenen özel resim var mı?
            // (Vehicle sınıfına getImagePath eklediğinizi varsayıyoruz)
            QString customPath = QString::fromStdString(v->getImagePath());
            bool imageLoaded = false;

            if (!customPath.isEmpty()) {
                if (pixmap.load(customPath)) {
                    imageLoaded = true;
                }
            }

            // B) Özel resim yoksa, Model adına göre varsayılanı dene
            if (!imageLoaded) {
                QString modelName = QString::fromStdString(v->getModel()).trimmed();
                QStringList tryPaths;
                tryPaths << ":/images/" + modelName.toLower() + ".jpg"; // clio.jpg
                tryPaths << ":/images/" + modelName + ".jpg";          // Clio.jpg
                tryPaths << ":/images/default_car.jpg";                // Varsayılan

                for (const QString& path : tryPaths) {
                    if (pixmap.load(path)) {
                        imageLoaded = true;
                        break;
                    }
                }
            }

            if (imageLoaded) {
                imageLabel->setPixmap(pixmap.scaled(100, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
            } else {
                imageLabel->setText("No Image");
                imageLabel->setStyleSheet("border: 1px solid gray;");
            }

            imageLabel->setAlignment(Qt::AlignCenter);
            vehicleTable->setCellWidget(row, 0, imageLabel);
            // --- RESİM YÜKLEME MANTIĞI (BİTİŞ) ---

            // Diğer sütunlar
            vehicleTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(v->getBrand())));
            vehicleTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(v->getModel())));
            vehicleTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(v->getType())));
            vehicleTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(v->getPlate())));
            vehicleTable->setItem(row, 5, new QTableWidgetItem(QString::number(v->getPricePerDay(), 'f', 2) + " TL"));
            vehicleTable->setItem(row, 6, new QTableWidgetItem(QString::fromStdString(v->getStatus())));
        }

        // Müşteri panelini de güncellemeyi unutmuyoruz
        updateCustomerPanelTable(customerVehicleTable);
    };

    auto updateCustomerTable = [&]() {
        customerTable->setRowCount(0);
        for(Customer* c : customers) {
            int row = customerTable->rowCount();
            customerTable->insertRow(row);
            customerTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(c->name)));
            customerTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(c->username)));
            customerTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(c->phone)));
            customerTable->setItem(row, 3, new QTableWidgetItem(QString::number(c->id)));
            customerTable->setItem(row, 4, new QTableWidgetItem(QString::number(c->totalRentals)));
        }
    };

    // --- GÜNCEL RENTAL TABLE (Renklendirme ve Buton Yönetimi) ---
    updateRentalTable = [&]() {
        rentalTable->setRowCount(0);
        rentalTable->setColumnCount(7);
        rentalTable->setHorizontalHeaderLabels({"Customer", "Vehicle", "Start Date", "End Date", "Total Cost", "Status", "Actions"});
        rentalTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        for(Rental* r : rentals) {
            int row = rentalTable->rowCount();
            rentalTable->insertRow(row);

            // Hücreleri Oluştur
            QTableWidgetItem* itemCust = new QTableWidgetItem(QString::fromStdString(r->customer->getName()));
            QTableWidgetItem* itemVeh = new QTableWidgetItem(QString::fromStdString(r->vehicle->getModel()));
            QTableWidgetItem* itemStart = new QTableWidgetItem(QString::fromStdString(r->startDate));
            QTableWidgetItem* itemEnd = new QTableWidgetItem(QString::fromStdString(r->endDate));
            QTableWidgetItem* itemCost = new QTableWidgetItem(QString::number(r->price, 'f', 2) + " TL");
            QTableWidgetItem* itemStatus = new QTableWidgetItem(QString::fromStdString(r->status));

            // --- RENKLENDİRME MANTIĞI ---
            QColor rowColor;
            if (r->status == "Reserved") {
                // Özel bir diyalog penceresi oluşturuyoruz
                QDialog actionDialog;
                actionDialog.setWindowTitle("Rezervasyon Yönetimi - #" + QString::number(r->id));
                actionDialog.setFixedSize(450, 400);
                QVBoxLayout *layout = new QVBoxLayout(&actionDialog);

                // 1. Durum Başlığı
                QLabel *header = new QLabel("<h3>📅 Rezervasyon İşlemleri</h3>");
                header->setAlignment(Qt::AlignCenter);
                layout->addWidget(header);

                // 2. Detaylı Bilgi Özeti (HTML formatında)
                // Tarih kontrolü: Bugün mü başlamalı?
                QDate startDate = QDate::fromString(QString::fromStdString(r->startDate), "dd/MM/yyyy");
                QDate today = QDate::currentDate();
                QString dateWarning = "";

                if (startDate > today) {
                    dateWarning = "<br><span style='color:orange;'>⚠️ DİKKAT: Rezervasyon tarihi henüz gelmedi!</span>";
                } else if (startDate < today) {
                    dateWarning = "<br><span style='color:red;'>⚠️ DİKKAT: Müşteri aracı almada geç kaldı!</span>";
                } else {
                    dateWarning = "<br><span style='color:green;'>✅ Tarih Uygun: Teslimat günü bugün.</span>";
                }

                QString infoHtml = QString(
                                       "<b>Müşteri:</b> %1<br>"
                                       "<b>Araç:</b> %2 (%3)<br>"
                                       "<b>Tarihler:</b> %4 - %5 Gün<br>"
                                       "<b>Toplam Tutar:</b> %6 TL<br>"
                                       "<b>Sigorta:</b> %7"
                                       "%8")
                                       .arg(QString::fromStdString(r->customer->getName()))
                                       .arg(QString::fromStdString(r->vehicle->getModel()))
                                       .arg(QString::fromStdString(r->vehicle->getPlate()))
                                       .arg(QString::fromStdString(r->startDate))
                                       .arg(r->days)
                                       .arg(r->price)
                                       .arg(QString::fromStdString(r->insurance->getName()))
                                       .arg(dateWarning); // Tarih uyarısını ekle

                QLabel *infoLabel = new QLabel(infoHtml);
                infoLabel->setStyleSheet("background-color: #f0f0f0; padding: 10px; border-radius: 5px;");
                layout->addWidget(infoLabel);

                layout->addSpacing(10);

                // 3. Aksiyon Butonları
                QPushButton *btnStart = new QPushButton("🔑 Aracı Teslim Et ve Kiralamayı Başlat");
                btnStart->setStyleSheet("background-color: #28a745; color: white; padding: 10px; font-weight: bold;");
                btnStart->setIcon(QIcon::fromTheme("media-playback-start")); // Varsa ikon

                QPushButton *btnCancel = new QPushButton("❌ Rezervasyonu İptal Et");
                btnCancel->setStyleSheet("background-color: #dc3545; color: white; padding: 8px;");

                QPushButton *btnClose = new QPushButton("Kapat");
                btnClose->setStyleSheet("padding: 5px;");

                layout->addWidget(btnStart);
                layout->addWidget(btnCancel);
                layout->addStretch(); // Boşluk bırak
                layout->addWidget(btnClose);

                // --- BUTON İŞLEVLERİ ---

                // A) BAŞLATMA İŞLEMİ
                QObject::connect(btnStart, &QPushButton::clicked, [&]() {
                    QMessageBox::StandardButton confirm = QMessageBox::question(
                        &actionDialog, "Onay",
                        "Araç müşteriye teslim ediliyor.\nDevam etmek istiyor musunuz?",
                        QMessageBox::Yes | QMessageBox::No);

                    if (confirm == QMessageBox::Yes) {
                        r->status = "Active";
                        // Not: Araç zaten rezerve olduğu için vehicle->status değişmesine gerek yok,
                        // ama garanti olsun diye 'Rented' veya 'Active' yapabilirsin.
                        // r->vehicle->setStatus("Rented");

                        saveRentalsToJson();
                        updateRentalTable(); // Tabloyu yenile

                        // Kullanıcıya şık bir bildirim
                        notificationSystem.sendNotification(r->customer->getUsername(), "Kiralama başladı. İyi yolculuklar!");
                        QMessageBox::information(&actionDialog, "Başarılı", "Kiralama aktif hale getirildi.");
                        actionDialog.accept();
                    }
                });

                // B) İPTAL İŞLEMİ
                QObject::connect(btnCancel, &QPushButton::clicked, [&]() {
                    // İptal nedeni sorma (Opsiyonel ama profesyonel)
                    bool ok;
                    QString reason = QInputDialog::getText(&actionDialog, "İptal Nedeni",
                                                           "Lütfen iptal nedenini giriniz:", QLineEdit::Normal,
                                                           "Müşteri vazgeçti", &ok);
                    if (ok && !reason.isEmpty()) {
                        r->status = "Cancelled";
                        r->endDate = "İptal: " + reason.toStdString();

                        // KRİTİK ADIM: Aracı boşa çıkar
                        r->vehicle->setStatus("Available");

                        saveRentalsToJson();
                        saveVehiclesToJson(); // Araç durumu değiştiği için kaydet
                        updateRentalTable();
                        updateVehicleTable(); // Araç tablosunu da güncelle

                        QMessageBox::information(&actionDialog, "İptal Edildi", "Rezervasyon iptal edildi, araç tekrar müsait.");
                        actionDialog.accept();
                    }
                });

                QObject::connect(btnClose, &QPushButton::clicked, &actionDialog, &QDialog::reject);

                actionDialog.exec();

                //rowColor = QColor(255, 200, 100); // Turuncu (Bekliyor)
            }
            else if (r->status == "Active") rowColor = QColor(144, 238, 144); // Açık Yeşil (Kirada)
            else if (r->status == "Completed") rowColor = QColor(220, 220, 220); // Gri (Bitti)
            else rowColor = QColor(255, 255, 255); // Beyaz

            // Rengi tüm hücrelere uygula
            itemCust->setBackground(rowColor);
            itemVeh->setBackground(rowColor);
            itemStart->setBackground(rowColor);
            itemEnd->setBackground(rowColor);
            itemCost->setBackground(rowColor);
            itemStatus->setBackground(rowColor);

            // Hücreleri Tabloya Ekle
            rentalTable->setItem(row, 0, itemCust);
            rentalTable->setItem(row, 1, itemVeh);
            rentalTable->setItem(row, 2, itemStart);
            rentalTable->setItem(row, 3, itemEnd);
            rentalTable->setItem(row, 4, itemCost);
            rentalTable->setItem(row, 5, itemStatus);

            // --- AKSİYON BUTONU ---
            QWidget *actionWidget = new QWidget();
            QHBoxLayout *actionLayout = new QHBoxLayout(actionWidget);
            QPushButton *actionBtn = new QPushButton();

            // Statüye göre buton metni ve durumu
            if (r->status == "Reserved") {
                actionBtn->setText("İptal / Başlat");
                actionBtn->setStyleSheet("background-color: #FF8C00; color: white;");
                actionBtn->setEnabled(true);
            }
            else if (r->status == "Active") {
                actionBtn->setText("Teslim Al");
                actionBtn->setStyleSheet("background-color: #008000; color: white;");
                actionBtn->setEnabled(true);
            }
            else {
                actionBtn->setText("Arşivlendi");
                actionBtn->setEnabled(false); // Tamamlananlar tıklanamaz
            }

            // Rental ID'yi butona kaydet
            actionBtn->setObjectName(QString::number(r->id));

            actionLayout->addWidget(actionBtn);
            actionLayout->setContentsMargins(2, 2, 2, 2);
            actionWidget->setLayout(actionLayout);

            rentalTable->setCellWidget(row, 6, actionWidget);
        }
    };





    auto updatePaymentTable = [&]() {
        paymentTable->setRowCount(0);
        // ...
        for(Payment* p : payments) {
            int row = paymentTable->rowCount();
            paymentTable->insertRow(row);

            // [0]: Rental ID
            paymentTable->setItem(row, 0, new QTableWidgetItem(QString::number(p->getRentalId()))); // DÜZELTİLDİ: Rental ID eklendi

            // [1]: Amount (Miktar)
            // Hatalı Eski: p->getType() -> Yeni: p->getAmount()
            paymentTable->setItem(row, 1, new QTableWidgetItem(QString::number(p->getAmount(), 'f', 2) + " TL")); // DÜZELTİLDİ

            // [2]: Method (Tip)
            // Hatalı Eski: p->getMethod() -> Yeni: p->getType()
            paymentTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(p->getType()))); // DÜZELTİLDİ

            // [3]: Date
            paymentTable->setItem(row, 3, new QTableWidgetItem(QString::fromStdString(p->getDate())));
        }
    };

    auto updateMaintenanceTable = [&]() {
        maintenanceTable->setRowCount(0);
        for(MaintenanceRecord* m : maintenanceRecords) {
            int row = maintenanceTable->rowCount();
            maintenanceTable->insertRow(row);
            maintenanceTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(m->vehicle->getPlate())));
            maintenanceTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(m->type)));
            maintenanceTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(m->description)));
            maintenanceTable->setItem(row, 3, new QTableWidgetItem(QString::number(m->cost, 'f', 2)));
            maintenanceTable->setItem(row, 4, new QTableWidgetItem(QString::fromStdString(m->date)));
        }
    };

    // --- Branch Tablosunu Güncelleme Fonksiyonu ---
    auto updateBranchTable = [&]() {
        branchTable->setRowCount(0);

        for (Branch* b : branches) {
            int row = branchTable->rowCount();
            branchTable->insertRow(row);

            // [0]: ID
            branchTable->setItem(row, 0, new QTableWidgetItem(QString::number(b->id)));

            // [1]: Şube Adı
            branchTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(b->name)));

            // [2]: Adres
            branchTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(b->address)));

            // [3]: Müdür (Username)
            // Manager sınıfının Employee sınıfından türediğini varsayarak username alıyoruz.
            QString managerUsername = QString::fromStdString(b->manager->getUsername());
            branchTable->setItem(row, 3, new QTableWidgetItem(managerUsername));
        }
    };

    auto updateEmployeeTable = [&]() {
        // employeeTable'ın tanımını göremedim, bu yüzden 5 sütunlu standart bir yapı kullanacağım:
        // Employee* sınıfının getUsername(), getName(), getRole() metodlarına sahip olduğunu varsayarız.
        // Eğer çalışan tablosunu oluşturmadıysanız, main'in başında bunu da eklemelisiniz.
        // QTableWidget *employeeTable = new QTableWidget(0, 5);

        employeeTable->setRowCount(0);
        employeeTable->setHorizontalHeaderLabels({"Kullanıcı Adı", "Ad Soyad", "Pozisyon", "Şube ID", "Total Rentals"});
        employeeTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

        for (Employee* e : employees) {
            int row = employeeTable->rowCount();
            employeeTable->insertRow(row);

            // [0] Kullanıcı Adı
            employeeTable->setItem(row, 0, new QTableWidgetItem(QString::fromStdString(e->getUsername())));
            // [1] Ad Soyad (Employee sınıfında getName() veya benzeri bir metot olduğunu varsayarız)
            employeeTable->setItem(row, 1, new QTableWidgetItem(QString::fromStdString(e->getName())));
            // [2] Pozisyon (Role)
            employeeTable->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(e->getRole())));
            // [3] Şube ID (Eğer Employee sınıfında getBranchId() veya benzeri bir metot varsa ekleyin)
            // Şimdilik boş bırakıyorum veya 'N/A' yazıyorum.
            employeeTable->setItem(row, 3, new QTableWidgetItem("N/A"));
            // [4] Toplam İşlem Sayısı (Eğer Employee sınıfında böyle bir özellik varsa)
            employeeTable->setItem(row, 4, new QTableWidgetItem("N/A"));
        }
    };

    auto updateAllTables = [&]() {
        updateVehicleTable();
        updateCustomerTable();
        updateEmployeeTable(); // <<< ARTIK BURADA HATA VERMEYECEK
        updateMaintenanceTable();
        updatePaymentTable();
        updateBranchTable();
        // Diğer tüm update...Table() lambdalarını buraya ekleyin.
    };



    /* ===================== LAYOUT KURULUMU ===================== */

    // 0. WELCOME SCREEN (Hoş Geldiniz Ekranı)

    // 0. WELCOME SCREEN (Hoş Geldiniz Ekranı)
    QWidget *welcomePage = new QWidget;
    QVBoxLayout *welcomeLayout = new QVBoxLayout(welcomePage);

    // DEĞİŞİKLİK 1: Kenar boşluklarını sıfırlıyoruz ki resim tam otursun
    welcomeLayout->setContentsMargins(0, 0, 0, 0);
    welcomeLayout->setSpacing(0);

    // Başlık (İstersen kaldırabilirsin, resmin üstünde şerit gibi durur)
    QLabel *welcomeLabel = new QLabel("<h2>🚗 Araç Kiralama Sistemine Hoş Geldiniz!</h2>");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet(
        "QLabel {"
        "   background-color: rgba(255, 255, 255, 0.9);" // Hafif saydam beyaz
        "   color: #333333;"
        "   padding: 15px;"
        "   border-bottom: 1px solid #ccc;"
        "}"
        );

    // DEĞİŞİKLİK 2: Görsel Ayarları (Responsive - Pencereye göre büyüyen)
    QLabel *welcomeImage = new QLabel();
    QPixmap welcomePixmap(getVehicleImagePath("welcome"));

    if (!welcomePixmap.isNull()) {
        welcomeImage->setPixmap(welcomePixmap);
        welcomeImage->setScaledContents(true); // Resmi etiketin boyutuna sığdır
        welcomeImage->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored); // Boyut kısıtlamasını kaldır
    } else {
        welcomeImage->setText("Görsel Yüklenemedi: images/welcome.jpg");
        welcomeImage->setAlignment(Qt::AlignCenter);
        welcomeImage->setStyleSheet("background-color: #ddd;");
    }

    // Butonlar
    QPushButton *welcome_loginBtn = new QPushButton("Giriş Yap");
    QPushButton *welcome_registerBtn = new QPushButton("Kayıt Ol");
    welcome_loginBtn->setFixedSize(150, 45);
    welcome_registerBtn->setFixedSize(150, 45);

    // Buton Stili
    QString btnStyle = "QPushButton { background-color: #333; color: white; border-radius: 5px; font-weight: bold; font-size: 14px; }"
                       "QPushButton:hover { background-color: #555; }";
    welcome_loginBtn->setStyleSheet(btnStyle);
    welcome_registerBtn->setStyleSheet(btnStyle);

    QHBoxLayout *welcomeButtonLayout = new QHBoxLayout;
    welcomeButtonLayout->setContentsMargins(0, 20, 0, 20); // Butonların etrafına biraz boşluk
    welcomeButtonLayout->addStretch(1);
    welcomeButtonLayout->addWidget(welcome_loginBtn);
    welcomeButtonLayout->addSpacing(20);
    welcomeButtonLayout->addWidget(welcome_registerBtn);
    welcomeButtonLayout->addStretch(1);

    // DEĞİŞİKLİK 3: Layout Düzeni (Resme öncelik veriyoruz)
    welcomeLayout->addWidget(welcomeLabel);     // En üste başlık
    welcomeLayout->addWidget(welcomeImage, 1);  // Ortaya resim ('1' sayesinde tüm boş alanı kaplar)
    welcomeLayout->addLayout(welcomeButtonLayout); // En alta butonlar

    // Arka plan
    welcomePage->setStyleSheet("background-color: #F8F8F8;");

    stackedWidget->addWidget(welcomePage); // Index 0




    // 1. LOGIN SCREEN (Giriş Yap Ekranı)
    QWidget *loginPage = new QWidget;
    QVBoxLayout *loginLayout = new QVBoxLayout(loginPage);

    QLabel *loginTitle = new QLabel("<h2>👤 Kullanıcı Girişi</h2>");
    loginTitle->setAlignment(Qt::AlignCenter);

    QPushButton *backToWelcomeBtn_L = new QPushButton("<- Geri");
    backToWelcomeBtn_L->setFixedSize(100, 30);
    QHBoxLayout *loginHeader = new QHBoxLayout;
    loginHeader->addWidget(backToWelcomeBtn_L);
    loginHeader->addStretch(1);
    loginHeader->addWidget(loginTitle);
    loginHeader->addStretch(1);

    QPushButton *loginBtn = new QPushButton("Giriş Yap");

    loginLayout->addLayout(loginHeader);
    loginLayout->addStretch(1);
    loginLayout->addWidget(new QLabel("Kullanıcı Adı:"));
    loginLayout->addWidget(login_usernameInput);
    loginLayout->addWidget(new QLabel("Şifre:"));
    loginLayout->addWidget(login_passwordInput);
    loginLayout->addWidget(loginBtn);
    loginLayout->addStretch(1);

    stackedWidget->addWidget(loginPage); // Index 1

    // 2. REGISTER SCREEN (Kayıt Ol Ekranı) - Sadece Müşteri
    QWidget *registerPage = new QWidget;
    QVBoxLayout *registerLayout = new QVBoxLayout(registerPage);

    QLabel *registerTitle = new QLabel("<h2>📝 Yeni Müşteri Kaydı</h2>");
    registerTitle->setAlignment(Qt::AlignCenter);

    QPushButton *backToWelcomeBtn_R = new QPushButton("<- Geri");
    backToWelcomeBtn_R->setFixedSize(100, 30);
    QHBoxLayout *registerHeader = new QHBoxLayout;
    registerHeader->addWidget(backToWelcomeBtn_R);
    registerHeader->addStretch(1);
    registerHeader->addWidget(registerTitle);
    registerHeader->addStretch(1);

    QPushButton *registerBtn = new QPushButton("Kayıt Ol (Müşteri)");

    registerLayout->addLayout(registerHeader);
    registerLayout->addStretch(1);
    registerLayout->addWidget(new QLabel("Ad Soyad:"));
    registerLayout->addWidget(register_nameInput);
    registerLayout->addWidget(new QLabel("Telefon:"));
    registerLayout->addWidget(register_phoneInput);
    registerLayout->addWidget(new QLabel("Kullanıcı Adı:"));
    registerLayout->addWidget(register_usernameInput);
    registerLayout->addWidget(new QLabel("Şifre:"));
    registerLayout->addWidget(register_passwordInput);
    registerLayout->addWidget(registerBtn);
    registerLayout->addStretch(1);

    stackedWidget->addWidget(registerPage); // Index 2

    // 3. YÖNETİM PANELİ (Admin Panel)
    QWidget *adminPage = new QWidget;
    QVBoxLayout *adminLayout = new QVBoxLayout(adminPage);

    QPushButton *adminLogoutBtn = new QPushButton("Çıkış Yap");
    adminLogoutBtn->setFixedSize(100, 30);

    QHBoxLayout *adminHeaderLayout = new QHBoxLayout;
    adminHeaderLayout->addStretch(1);
    adminHeaderLayout->addWidget(adminLogoutBtn);

    // Vehicles Tab (Layout)
    QWidget *vehiclesTab = new QWidget();
    vehicleTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QHBoxLayout *vTopLayout = new QHBoxLayout();
    vTopLayout->addWidget(new QLabel("VEHICLE MANAGEMENT"));
    vTopLayout->addStretch(1);
    vTopLayout->addWidget(addVehicleBtn);
    QVBoxLayout *vehiclesLayout = new QVBoxLayout(vehiclesTab);
    vehiclesLayout->addLayout(vTopLayout);
    vehiclesLayout->addWidget(vehicleTable);

    // Customers Tab (Layout)
    QWidget *customersTab = new QWidget();
    customerTable->setHorizontalHeaderLabels({"Full Name", "Username", "Phone", "ID", "Total Rentals"});
    customerTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QVBoxLayout *customersLayout = new QVBoxLayout(customersTab);
    customersLayout->addWidget(customerTable);

    // Rentals Tab (Layout)
    QWidget *rentalTab = new QWidget();
    rentalTable->setHorizontalHeaderLabels({"Customer", "Vehicle", "Start Date", "End Date", "Total Cost", "Status", "Actions"});
    rentalTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    QVBoxLayout *rentalLayout = new QVBoxLayout(rentalTab);
    rentalLayout->addWidget(rentalTable);

    // Payments Tab (Layout)
    QWidget *paymentsTab = new QWidget();
    paymentTable->setHorizontalHeaderLabels({"Rental ID", "Amount (TL)", "Method", "Date"});
    paymentTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    recordPaymentBtn->setStyleSheet("background-color: #008000; color: white;");
    QVBoxLayout *paymentsLayout = new QVBoxLayout(paymentsTab);
    paymentsLayout->addWidget(recordPaymentBtn);
    paymentsLayout->addWidget(paymentTable);

    // Maintenance Tab (Layout)
    QWidget *maintenanceTab = new QWidget();
    maintenanceTable->setHorizontalHeaderLabels({"Vehicle Plate", "Type", "Description", "Cost ($)", "Date"});
    maintenanceTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    addMaintenanceBtn->setStyleSheet("background-color: #A9A9A9; color: black;");
    QVBoxLayout *maintenanceLayout = new QVBoxLayout(maintenanceTab);
    maintenanceLayout->addWidget(addMaintenanceBtn);
    maintenanceLayout->addWidget(maintenanceTable);

    adminLayout->addLayout(adminHeaderLayout);
    adminLayout->addWidget(adminTabs);
    stackedWidget->addWidget(adminPage); // Index 3

    // 4. MÜŞTERİ PANELİ (Customer Panel)
    QWidget *customerPage = new QWidget;
    QVBoxLayout *customerPanelLayout = new QVBoxLayout(customerPage);

    QPushButton *customerLogoutBtn = new QPushButton("Çıkış Yap");
    customerLogoutBtn->setFixedSize(100, 30);

    QHBoxLayout *customerHeaderLayout = new QHBoxLayout;
    customerHeaderLayout->addStretch(1);
    customerHeaderLayout->addWidget(customerLogoutBtn);

    customerPanelLayout->addWidget(customerVehicleTable); // Tabloyu ekle

    QGroupBox *rentBox = new QGroupBox("Kiralamayı Tamamla");
    QVBoxLayout *rentLayout = new QVBoxLayout(rentBox);
    rentDaysInput->setPlaceholderText("Kaç Gün Kiralanacak?");
    startDateEdit->setDisplayFormat("dd/MM/yyyy");
    rentCarBtn->setStyleSheet("background-color: #008000; color: white;");

    for (Insurance* ins : insuranceOptions) {
        insuranceCombo->addItem(QString::fromStdString(ins->getName()) + " (" + QString::number(ins->getDailyCost()) + " TL/day)");
    }

    rentLayout->addWidget(new QLabel("Başlangıç Tarihi:"));
    rentLayout->addWidget(startDateEdit);
    rentLayout->addWidget(new QLabel("Sigorta Seçeneği:"));
    rentLayout->addWidget(insuranceCombo);
    rentLayout->addWidget(new QLabel("Kiralama Gün Sayısı:"));
    rentLayout->addWidget(rentDaysInput);
    rentLayout->addWidget(rentCarBtn);

    customerPanelLayout->addWidget(rentBox);

    QTabWidget *customerTabs = new QTabWidget();
    customerTabs->addTab(customerPage, "🛒 Araçları Görüntüle ve Kirala");

    QWidget *customerTabContainer = new QWidget;
    QVBoxLayout *customerContainerLayout = new QVBoxLayout(customerTabContainer);
    customerContainerLayout->addLayout(customerHeaderLayout);
    customerContainerLayout->addWidget(customerTabs);

    stackedWidget->addWidget(customerTabContainer); // Index 4

    /* ===================== BAĞLANTILAR ===================== */

    // --- Geçiş Bağlantıları ---
    QObject::connect(welcome_loginBtn, &QPushButton::clicked, [&]() {
        stackedWidget->setCurrentIndex(1); // Login'e geç
    });
    QObject::connect(welcome_registerBtn, &QPushButton::clicked, [&]() {
        // Kayıt formu alanlarını temizle
        register_usernameInput->clear();
        register_passwordInput->clear();
        register_nameInput->clear();
        register_phoneInput->clear();
        stackedWidget->setCurrentIndex(2); // Register'a geç
    });
    QObject::connect(backToWelcomeBtn_L, &QPushButton::clicked, [&]() {
        stackedWidget->setCurrentIndex(0); // Welcome'a geri
    });
    QObject::connect(backToWelcomeBtn_R, &QPushButton::clicked, [&]() {
        stackedWidget->setCurrentIndex(0); // Welcome'a geri
    });
    QObject::connect(adminLogoutBtn, &QPushButton::clicked, [&]() {
        stackedWidget->setCurrentIndex(0); // Çıkış yap -> Welcome
        login_usernameInput->clear();
        login_passwordInput->clear();
        rentDaysInput->clear();
    });
    QObject::connect(customerLogoutBtn, &QPushButton::clicked, [&]() {
        stackedWidget->setCurrentIndex(0); // Çıkış yap -> Welcome
        login_usernameInput->clear();
        login_passwordInput->clear();

        // YENİ EKLEME
        rentDaysInput->clear();

        // OPSİYONEL: Ayrıca diğer kiralama form alanlarını da temizleyebilirsiniz.
        startDateEdit->setDate(QDate::currentDate());
        insuranceCombo->setCurrentIndex(0);
    });
    // --- Araç Ekleme İşlemi (Manager Paneli) ---
    QObject::connect(addVehicleBtn, &QPushButton::clicked, [&]() {
        // Sadece yönetici rolü için butonu çalıştır
        string loggedInUser = login_usernameInput->text().toStdString();
        Employee* employee = findEmployeeByUsername(loggedInUser);

        if (employee && employee->getRole() == "Manager") {
            createAddVehicleDialog(vehicleTable, branches, updateVehicleTable, notificationSystem);
        } else {
            QMessageBox::warning(adminPage, "Yetki Hatası", "Sadece Yöneticiler araç ekleyebilir.");
        }
    });
    // --- Bakım Kaydı Ekleme İşlemi (Manager Paneli) ---
    QObject::connect(addMaintenanceBtn, &QPushButton::clicked, [&]() {
        // Sadece yönetici rolü için butonu çalıştır
        string loggedInUser = login_usernameInput->text().toStdString();
        Employee* employee = findEmployeeByUsername(loggedInUser);

        if (employee && employee->getRole() == "Manager") {
            createAddMaintenanceDialog(maintenanceTable, vehicles, updateMaintenanceTable, updateVehicleTable, updateAllTables,notificationSystem);

        } else {
            QMessageBox::warning(adminPage, "Yetki Hatası", "Sadece Yöneticiler bakım kaydı ekleyebilir.");
        }
    });

    // --- Şube Ekleme İşlemi (Manager Paneli) ---
    QObject::connect(addBranchBtn, &QPushButton::clicked, [&]() {
        // Sadece yönetici rolü için butonu çalıştır
        string loggedInUser = login_usernameInput->text().toStdString();
        Employee* employee = findEmployeeByUsername(loggedInUser);

        if (employee && employee->getRole() == "Manager") {
            // Tüm tabloları güncelleyen lambdayı createAddBranchDialog'a gönderiyoruz.
            // updateVehicleTable'ı doğrudan göndererek tüm tabloları dolaylı yoldan güncellemiş oluruz.
            createAddBranchDialog(updateAllTables);
        } else {
            QMessageBox::warning(adminPage, "Yetki Hatası", "Sadece Yöneticiler yeni şube ekleyebilir.");
        }
    });



    // --- Kayıt İşlemi ---
    QObject::connect(registerBtn, &QPushButton::clicked, [&]() {
        QString u = register_usernameInput->text();
        QString p = register_passwordInput->text();
        QString n = register_nameInput->text();
        QString phone = register_phoneInput->text();

        if (u.isEmpty() || p.isEmpty() || n.isEmpty() || phone.isEmpty()) {
            QMessageBox::warning(registerPage, "Hata", "Tüm alanlar boş bırakılamaz.");
            return;
        }

        if (findCustomerByUsername(u.toStdString()) || findEmployeeByUsername(u.toStdString())) {
            QMessageBox::warning(registerPage, "Hata", "Bu kullanıcı adı zaten sistemde mevcut.");
            return;
        }

        int id = QRandomGenerator::global()->generate() % 9000 + 1000;
        customers.push_back(new Customer(n.toStdString(), phone.toStdString(), id, u.toStdString(), p.toStdString()));

        saveCustomersToJson();

        QMessageBox::information(registerPage, "Başarılı", "Müşteri kaydı başarıyla oluşturuldu! Şimdi giriş yapabilirsiniz.");
        notificationSystem.sendNotification(u.toStdString(), "Sistemimize hoş geldiniz.");

        // Kayıt başarılı, giriş ekranına geri dön
        stackedWidget->setCurrentIndex(1);
    });

    // --- Giriş Yap İşlemi ---
    QObject::connect(loginBtn, &QPushButton::clicked, [&]() {
        string enteredUsername = login_usernameInput->text().toStdString();
        string enteredPassword = login_passwordInput->text().toStdString();

        // 1. Müşteri Kontrolü
        Customer* customer = findCustomerByUsername(enteredUsername);
        if (customer && customer->getPassword() == enteredPassword) {
            updateCustomerPanelTable(customerVehicleTable);
            stackedWidget->setCurrentIndex(4); // Customer Page
            return;
        }

        // 2. Çalışan Kontrolü
        Employee* employee = findEmployeeByUsername(enteredUsername);
        if (employee && employee->getPassword() == enteredPassword) {

            updateVehicleTable();
            updateCustomerTable();
            updateRentalTable();
            updatePaymentTable();
            updateMaintenanceTable();

            // Sekmeleri sıfırlama ve role göre ayarlama
            adminTabs->clear();
            adminTabs->addTab(rentalTab, "📝 Rentals");
            adminTabs->addTab(paymentsTab, "💳 Payments");

            if (employee->getRole() == "Manager") {
                adminTabs->insertTab(0, vehiclesTab, "🚗 Vehicles");
                adminTabs->insertTab(1, customersTab, "👤 Customers");
                adminTabs->addTab(maintenanceTab, "⚙ Maintenance");
                adminTabs->addTab(branchesTab, "🏢 Branches");
            } else if (employee->getRole() == "Sales") {
                // Sales (Satış) personeli sadece kiralama ve ödeme işlemlerini yönetir.
            }
            updateBranchTable();
            stackedWidget->setCurrentIndex(3); // Admin Page
            return;
        }

        QMessageBox::warning(loginPage, "Hata", "Geçersiz giriş bilgileri!");
    });


    // --- Kiralama İşlemi (Müşteri Paneli) ---
    QObject::connect(rentCarBtn, &QPushButton::clicked, [&]() {

        Customer* currentCustomer = findCustomerByUsername(login_usernameInput->text().toStdString());

        if (!currentCustomer) {
            QMessageBox::warning(customerPage, "Hata", "Giriş yapan müşteri bilgisi bulunamadı!");
            return;
        }

        int selectedRow = customerVehicleTable->currentRow();
        if (selectedRow == -1) {
            QMessageBox::warning(customerPage, "Hata", "Lütfen önce kiralamak istediğiniz aracı listeden seçin!");
            return;
        }

        // Seçim Parametrelerini Toplama
        QString plate = customerVehicleTable->item(selectedRow, 4)->text();
        int days = rentDaysInput->text().toInt();
        QDate selectedDate = startDateEdit->date(); // YENİ: QDate olarak alıyoruz
        QString sDate = selectedDate.toString("dd/MM/yyyy");

        if (days <= 0) {
            QMessageBox::warning(customerPage, "Hata", "Lütfen geçerli gün sayısı girin!");
            return;
        }

        Vehicle* selectedVehicle = findVehicle(plate.toStdString());

        // Sigortayı Bulma
        QString selectedInsuranceText = insuranceCombo->currentText();
        string insuranceName = selectedInsuranceText.split(" (").at(0).toStdString();
        Insurance* selectedInsurance = nullptr;
        for(Insurance* ins : insuranceOptions) {
            if (ins->getName() == insuranceName) {
                selectedInsurance = ins;
                break;
            }
        }

        if (!selectedVehicle || !selectedInsurance) {
            QMessageBox::critical(customerPage, "Hata", "Araç veya sigorta seçimi geçersiz.");
            return;
        }

        // --- DEĞİŞİKLİK BURADA BAŞLIYOR ---

        // 1. Önce aracın Bakımda olup olmadığını kontrol edelim (Bakım tarihi belirsizse kiralanamaz)
        if (selectedVehicle->getStatus() == "Maintenance") {
            QMessageBox::warning(customerPage, "Uyarı", "Bu araç şu anda bakımda olduğu için kiralanamaz.");
            return;
        }

        // 2. Tarih Çakışma Kontrolü (Yukarıda yazdığımız fonksiyonu çağırıyoruz)
        if (!isVehicleAvailableForDates(selectedVehicle, selectedDate, days)) {
            QMessageBox::warning(customerPage, "Müsait Değil",
                                 "Seçilen araç, girdiğiniz tarih aralığında başka bir rezervasyon nedeniyle doludur.\n"
                                 "Lütfen farklı bir tarih veya araç seçiniz.");
            return;
        }








        // --- Maliyet Hesaplama ---
        double vehicleCost = selectedVehicle->calculatePrice(days);
        double insuranceCost = selectedInsurance->getDailyCost() * days;
        double totalPrice = vehicleCost + insuranceCost;

        // --- Rezervasyon Ücreti (%25) Hesaplama ---
        double depositRate = 0.25;
        double depositAmount = totalPrice * depositRate;

        // --- Onay Diyaloğu Oluşturma ---
        QDialog confirmationDialog(customerPage);
        confirmationDialog.setWindowTitle("Rezervasyon Ön Ödeme ve Onay");
        confirmationDialog.resize(500, 400); // Diyalog boyutunu ayarla
        QVBoxLayout *dialogLayout = new QVBoxLayout(&confirmationDialog);

        QString summary = QString("<h3>Rezervasyon Özeti</h3>") +
                          QString("<b>Müşteri:</b> %1<br>").arg(QString::fromStdString(currentCustomer->getName())) +
                          QString("<b>Araç:</b> %1 (%2)<br>").arg(QString::fromStdString(selectedVehicle->getModel()), QString::fromStdString(selectedVehicle->getPlate())) +
                          QString("<b>Kiralama Gün Sayısı:</b> %1 gün<br>").arg(days) +
                          QString("<b>Başlangıç Tarihi:</b> %1<br>").arg(sDate) +
                          QString("<b>Sigorta Tipi:</b> %1 (%2 TL/gün)<br><br>").arg(QString::fromStdString(selectedInsurance->getName()), QString::number(selectedInsurance->getDailyCost(), 'f', 2)) +
                          QString("<hr>") +
                          QString("<b>Toplam Kiralama Maliyeti:</b> %1 TL<br>").arg(QString::number(totalPrice, 'f', 2)) +
                          QString("<b>Ön Ödeme Oranı:</b> %25<br>") +
                          QString("<h3>Ödenmesi Gereken Rezervasyon Ücreti: %1 TL</h3>").arg(QString::number(depositAmount, 'f', 2));

        QLabel *summaryLabel = new QLabel(summary);
        summaryLabel->setTextFormat(Qt::RichText);
        dialogLayout->addWidget(summaryLabel);

        // Ödeme Metodu Seçimi
        QComboBox *paymentMethod = new QComboBox();
        paymentMethod->addItem("Nakit (Cash)");
        paymentMethod->addItem("Kredi Kartı (Credit Card)");
        // paymentMethod->addItem("Online Transfer");
        dialogLayout->addWidget(new QLabel("Select Payment Method:"));
        dialogLayout->addWidget(paymentMethod);

        // Butonlar
        QPushButton *confirmBtn = new QPushButton(QString("%1 TL Ödeyerek Rezervasyonu Onayla").arg(QString::number(depositAmount, 'f', 2)));
        QPushButton *cancelBtn = new QPushButton("Cancel");
        confirmBtn->setStyleSheet("background-color: #FFA500; color: white;");

        QHBoxLayout *btnLayout = new QHBoxLayout();
        btnLayout->addWidget(confirmBtn);
        btnLayout->addWidget(cancelBtn);
        dialogLayout->addLayout(btnLayout);

        // --- Onay Butonu Bağlantısı (Rezervasyon ve Ön Ödeme İşlemini Gerçekleştirme) ---
        // --- Onay Butonu Bağlantısı ---
        QObject::connect(confirmBtn, &QPushButton::clicked, [&, depositAmount, totalPrice, selectedVehicle, currentCustomer, selectedInsurance, days]() {

            // 1. Kiralama Kaydı Oluştur (Status: Reserved)
            int newRentalId = rentals.empty() ? 1 : rentals.back()->id + 1;
            Rental* r = new Rental(newRentalId, currentCustomer, selectedVehicle, days, startDateEdit->date().toString("dd/MM/yyyy").toStdString(), selectedInsurance);
            r->status = "Reserved";

            // 2. Ödeme Yöntemini Kontrol Et
            QString methodText = paymentMethod->currentText();
            string currentDate = QDate::currentDate().toString("dd/MM/yyyy").toStdString();
            Payment* p = nullptr;

            // --- NAKİT ÖDEME ---
            if (methodText.contains("Nakit")) {
                p = new CashPayment(depositAmount, currentDate, newRentalId);
            }
            // --- KREDİ KARTI ÖDEME ---
            else if (methodText.contains("Kredi Kartı")) {

                // Diyalog Penceresi Oluştur
                QDialog dialog;
                dialog.setWindowTitle("Kart Bilgileri");
                dialog.setFixedSize(300, 250);
                QFormLayout form(&dialog);

                QLineEdit *cardHolder = new QLineEdit();
                QLineEdit *cardNumber = new QLineEdit();
                cardNumber->setInputMask("9999 9999 9999 9999");
                QLineEdit *expiry = new QLineEdit();
                expiry->setInputMask("99/99");
                QLineEdit *cvv = new QLineEdit();
                cvv->setEchoMode(QLineEdit::Password);

                form.addRow("Kart Sahibi:", cardHolder);
                form.addRow("Kart No:", cardNumber);
                form.addRow("SKT:", expiry);
                form.addRow("CVV:", cvv);

                QPushButton *payBtn = new QPushButton("Ödemeyi Tamamla");
                payBtn->setStyleSheet("background-color: #007BFF; color: white;");
                form.addRow(payBtn);

                // Butona tıklandığında yapılacaklar
                QObject::connect(payBtn, &QPushButton::clicked, [&]() {
                    if (cardNumber->text().length() < 19) {
                        QMessageBox::warning(&dialog, "Hata", "Kart numarası eksik!");
                        return;
                    }

                    // Başarılı Bildirimi
                    QMessageBox::information(&dialog, "Başarılı", "Ödeme onaylandı! Rezervasyon tamamlandı.");
                    dialog.accept(); // Pencereyi kapatır
                });

                // Pencereyi aç ve sonucunu bekle
                if (dialog.exec() == QDialog::Accepted) {
                    string lastFour = cardNumber->text().right(4).toStdString();
                    p = new CardPayment(depositAmount, currentDate, lastFour, newRentalId);
                } else {
                    // Kullanıcı çarpıya basıp iptal ettiyse, Rental nesnesini temizlememiz gerekir
                    delete r;
                    return;
                }
            }

            // 3. Eğer Ödeme Başarılıysa (p nesnesi oluştuysa) Kaydet
            if (p) {
                rentals.push_back(r);      // Kiralamayı listeye ekle
                payments.push_back(p);     // Ödemeyi listeye ekle

                selectedVehicle->setStatus("Reserved"); // Araç durumunu güncelle
                saveCustomersToJson();     // Müşteriyi (rental sayısını) güncelle
                saveVehiclesToJson();
                saveRentalsToJson();




                // Ana Rezervasyon Penceresini Kapat
                confirmationDialog.accept();
            }
        });
        // --- RENTAL TABLO AKSİYONLARI (GÜNCEL) ---
        QObject::connect(rentalTable, &QTableWidget::cellClicked, [&](int row, int column) {
            // Sadece buton sütununa (6. sütun) tıklanırsa işlem yap
            if (column != 6) return;

            QWidget *widget = rentalTable->cellWidget(row, column);
            if (!widget) return;
            QPushButton *btn = widget->findChild<QPushButton*>();
            if (!btn || !btn->isEnabled()) return;

            int rentalId = btn->objectName().toInt();
            Rental* rental = findRentalById(rentalId);

            if (!rental) return;

            // --- SENARYO A: REZERVE İŞLEMİ (Reserved) ---
            if (rental->status == "Reserved") {
                QMessageBox::StandardButton reply;
                reply = QMessageBox::question(rentalTable, "İşlem Seçimi",
                                              "Bu araç rezerve durumdadır. Ne yapmak istersiniz?\n\n"
                                              "Yes -> Kiralamayı Başlat (Aracı Teslim Et)\n"
                                              "No  -> Rezervasyonu İptal Et",
                                              QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

                if (reply == QMessageBox::Yes) {
                    // Kiralamayı Aktif Yap
                    rental->status = "Active";
                    rental->vehicle->setStatus("Available"); // Teknik olarak kirada ama sistemde 'Available' yerine 'Rented' olması daha doğru olurdu, şimdilik böyle bırakıyoruz.
                    // NOT: Eğer sisteminizde 'Rented' statüsü varsa burayı "Rented" yapın.

                    updateRentalTable();
                    updateVehicleTable();
                    QMessageBox::information(rentalTable, "Başarılı", "Kiralama süreci başlatıldı.");
                }
                else if (reply == QMessageBox::No) {
                    // İptal Et
                    rental->status = "Cancelled";
                    rental->endDate = "Cancelled";
                    rental->vehicle->setStatus("Available"); // Araç boşa çıkar

                    updateRentalTable();
                    updateVehicleTable();
                    QMessageBox::information(rentalTable, "İptal", "Rezervasyon iptal edildi.");
                }
            }
            // --- SENARYO B: AKTİF KİRALAMA BİTİRME (Active) ---
            else if (rental->status == "Active") {
                // Burası eski "Teslim Al" mantığıyla aynıdır
                QMessageBox::StandardButton reply = QMessageBox::question(rentalTable, "Teslim Al",
                                                                          QString("'%1' plakalı aracın kiralamasını sonlandırıp teslim almak istiyor musunuz?")
                                                                              .arg(QString::fromStdString(rental->vehicle->getPlate())),
                                                                          QMessageBox::Yes | QMessageBox::No);

                if (reply == QMessageBox::Yes) {
                    // Kalan ödeme ekranı açılır (Basitleştirilmiş)
                    double remaining = rental->price * 0.75; // %75 kalan varsayımı

                    // Ödeme Diyaloğu
                    QDialog pDialog;
                    QVBoxLayout l(&pDialog);
                    l.addWidget(new QLabel("Kalan Tutar: " + QString::number(remaining) + " TL"));
                    QPushButton* b = new QPushButton("Tahsil Et ve Bitir");
                    l.addWidget(b);

                    QObject::connect(b, &QPushButton::clicked, [&]() {
                        // Ödemeyi kaydet
                        Payment* p = new CashPayment(remaining, QDate::currentDate().toString("dd/MM/yyyy").toStdString(), rentalId);
                        payments.push_back(p);

                        // Durumları Güncelle
                        rental->status = "Completed";
                        rental->endDate = QDate::currentDate().toString("dd/MM/yyyy").toStdString();
                        rental->vehicle->setStatus("Available");

                        updateAllTables(); // Tüm tabloları yenile
                        pDialog.accept();
                    });
                    pDialog.exec();
                }
            }
        });







        QObject::connect(cancelBtn, &QPushButton::clicked, &confirmationDialog, &QDialog::reject);

        confirmationDialog.exec();
    });




    // --- Uygulamayı Başlatma ---
    stackedWidget->setCurrentIndex(0); // Başlangıçta Welcome ekranını göster
    mainWindow.show();
    return app.exec();
}

