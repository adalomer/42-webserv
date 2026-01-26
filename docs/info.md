# Webserver Projesi - Kapsamlı Teorik Dokümantasyon

## 1. Proje Genel Bakış

### 1.1 Webserver Nedir?

Webserver, HTTP protokolü üzerinden istemcilere (browser, curl, postman vb.) web sayfaları, dosyalar ve veriler sunan bir ağ uygulamasıdır. 42 okullarındaki bu projede, C++98 standartlarında sıfırdan bir webserver geliştirilmesi hedeflenmektedir.

**Temel İşlevler:**

- Port dinleme ve socket oluşturma
- HTTP isteklerini parsing etme
- HTTP yanıtları oluşturma
- Statik dosya servisi
- CGI desteği
- Çoklu bağlantı yönetimi (I/O multiplexing)

## 2. Temel TCP/IP ve HTTP Teorisi

### 2.1 TCP/IP Protokol Yığını

```
Uygulama Katmanı (HTTP, FTP, SMTP)
    ↓
Taşıma Katmanı (TCP/UDP)
    ↓
Ağ Katmanı (IP)
    ↓
Fiziksel Katman (Ethernet, WiFi)
```

### 2.2 HTTP (HyperText Transfer Protocol)

HTTP, istemci-sunucu modelinde çalışan, durumsuz (stateless) bir uygulama katmanı protokolüdür.

**HTTP İsteği Yapısı:**

```
GET /index.html HTTP/1.1\r\n
Host: localhost:8080\r\n
User-Agent: Mozilla/5.0\r\n
Accept: text/html\r\n
\r\n
```

**HTTP Yanıt Yapısı:**

```
HTTP/1.1 200 OK\r\n
Content-Type: text/html\r\n
Content-Length: 1256\r\n
\r\n
<!DOCTYPE html>
<html>
...
</html>
```

## 3. Temel Kavramların Teorik Anlatımı

### 3.1 Neden Socket Bağlama Yapıyoruz?

#### Socket Nedir?

Socket, ağ üzerindeki farklı cihazlar arasında iletişim kurmak için kullanılan bir soyutlama katmanıdır. Telefon hattı gibi düşünebilirsiniz - bir ucu sizde, diğer ucu karşı tarafta.

#### Socket Bağlama (Binding) Neden Gereklidir?

**Analog Örnek:** Bir şirket açtığınızı düşünün. Şirketinizin müşteriler tarafından bulunabilmesi için:

1. Bir bina kiralamanız (socket oluşturma) → `socket()`
2. Binaya bir adres vermeniz (IP ve port atama) → `bind()`
3. Kapıyı açıp müşteri beklemeye başlamanız → `listen()`

**Teknik Amaçları:**

- **Adresleme:** "Ben 8080 numaralı portta dinliyorum" demek
- **Port Rezervasyonu:** Diğer programların aynı portu kullanmasını engellemek
- **Gelen Paketleri Yönlendirme:** İşletim sistemi, gelen TCP paketlerini doğru programa iletir

### 3.2 Neden HTTP Parser Yazıyoruz?

#### HTTP Parser'in Amacı:

İstemciden gelen ham byte dizisini anlamlı HTTP isteklerine çevirmek.

**Örnek Senaryo:**

```
İstemci gönderiyor: "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n"

Bu ham string'i parçalayıp:
- Method: GET
- URI: /index.html
- HTTP Version: 1.1
- Host Header: localhost

şeklinde yapılandırılmış veriye çevirir.
```

#### Neden Hazır Kütüphane Kullanmıyoruz?

- **Eğitim Amacı:** TCP/IP stack'in nasıl çalıştığını anlamak
- **Kontrol:** Her detayı kontrol edebilmek
- **Performans:** Özel ihtiyaçlar için optimize edebilmek

#### Parsing Süreci:

1. **Request Line Parsing:** İlk satırı boşluklara göre böl (GET /index.html HTTP/1.1)
2. **Header Parsing:** Her satırı `:` karakterine göre ayır (Key: Value formatı)
3. **Body Parsing:** Header'dan sonraki kısmı al (Content-Length'e göre)

### 3.3 Neden I/O Multiplexing (select/poll/epoll) Kullanıyoruz?

#### Problem:

Basit webserver'lar bir anda sadece bir bağlantıyı işleyebilir. 100 kişi aynı anda bağlanırsa, 99'u bekler.

#### Çözüm:

I/O multiplexing ile aynı anda birden fazla bağlantıyı izleyebiliriz.

**Restoran Analojisi:**

- **Tek Garsonlu Restoran:** Her masaya sırayla gider → Yavaş
- **Çoklu Sipariş Sistemi:** Tüm masalardan sipariş alır, mutfağa iletir, hazır olanları masalara götürür → Verimli

#### select() Nasıl Çalışır?

1. Tüm aktif bağlantıların dosya tanımlayıcılarını (file descriptors) bir listeye koy
2. İşletim sistemine sor: "Bunlardan hangisi hazır?"
3. Hazır olanları işle, diğerlerini bekle

### 3.4 Neden CGI (Common Gateway Interface) Implemente Ediyoruz?

#### CGI'nin Amacı:

Statik HTML dosyalarından daha fazlasını sunabilmek.

**Örnek Senaryolar:**

- Kullanıcıdan form verisi alıp veritabanına kaydetmek
- Ziyaretçi sayacı yapmak
- Dinamik içerik üretmek (hava durumu, borsa verileri)

#### Çalışma Prensibi:

```
İstemci → Webserver → CGI Programı (Python/PHP) → Webserver → İstemci
```

**Örnek Akış:**

1. Kullanıcı `/search?q=42` URL'sine gider
2. Webserver, search.py CGI programını çalıştırır
3. CGI programı query parametresini alır, veritabanında arar
4. Sonuçları HTML olarak üretir
5. Webserver bu HTML'i kullanıcıya gönderir

### 3.5 Neden Configuration Parser Yazıyoruz?

#### İhtiyaç:

Webserver'ın davranışını kod değiştirmeden yapılandırabilmek.

**Yapılandırma Dosyası Örneği:**

```
server {
    port: 8080;                    # Hangi portta çalışacak?
    root: /var/www;               # Dosyalar nerede?
    max_clients: 100;             # Kaç bağlantı kabul edecek?
    timeout: 30;                  # Bağlantı ne kadar açık kalacak?
}
```

**Avantajları:**

- **Esneklik:** Port, dizin, limitler kolayca değiştirilebilir
- **Çoklu Site:** Birden fazla website aynı sunucuda çalışabilir
- **Güvenlik:** Kök dizin gibi kritik ayarlar kod dışında tutulabilir

### 3.6 Neden Thread/Process Yönetimi Gerekli?

#### Performans Problemi:

CGI programları (Python, PHP) yavaş çalışabilir. Webserver'ın diğer isteklere cevap vermesi engellenmemeli.

#### Çözümler:

**Forking:** Her CGI için yeni process oluştur

- **Avantaj:** İzole çalışır, birisi çökerse diğerleri etkilenmez
- **Dezavantaj:** Yüksek kaynak tüketimi

**Threading:** Her bağlantı için thread oluştur

- **Avantaj:** Hafif, hızlı
- **Dezavantaj:** Paylaşılan bellek, race condition riski

### 3.7 Neden Error Handling Önemli?

#### Webserver'ın Karşılaşabileceği Hatalar:

- **Dosya Bulunamadı:** 404 Not Found
- **Yetkisiz Erişim:** 403 Forbidden
- **Geçersiz İstek:** 400 Bad Request
- **Sunucu Hatası:** 500 Internal Server Error

#### Error Handling'in Amacı:

- **Kullanıcı Deneyimi:** Anlamlı hata mesajları
- **Güvenlik:** Sistem detaylarını sızdırmamak
- **Debugging:** Hataların loglanması

### 3.8 Neden Logging Mekanizması Kuruyoruz?

#### Log Kayıtlarının Önemi:

- **Debugging:** "Neden çalışmıyor?" sorusuna cevap
- **Monitoring:** Sunucu yükünü izleme
- **Güvenlik:** Şüpheli aktiviteleri tespit etme

**Log Formatı Örneği:**

```
[2024-01-15 10:30:25] [INFO] Client connected from 192.168.1.10
[2024-01-15 10:30:25] [DEBUG] Request: GET /index.html
[2024-01-15 10:30:25] [ERROR] File not found: /var/www/notexist.html
```

### 3.9 Neden Buffer Management Gerekli?

#### Problem:

TCP, veriyi paketler halinde gönderir. Bir HTTP isteği birden fazla pakete bölünebilir.

**Senaryo:**

```
Paket 1: "GET /index.html HTTP/1.1\r\nHost: local"
Paket 2: "host\r\nUser-Agent: Mozilla\r\n\r\n"
```

#### Buffer'ın Rolü:

- **Paket Birleştirme:** Gelen paketleri birleştirip tam istek oluşturma
- **Performans:** Sık sık disk/dosya okuma yerine memory'de tutma
- **Akış Kontrolü:** Büyük dosya gönderimlerini yönetme

## 4. Sistem Mimarisi

### 4.1 Ana Bileşenler

```
[İstemci] 
    ↓ (HTTP Request)
[Socket Layer]
    ↓ (TCP Packets)
[I/O Multiplexer (select/poll)]
    ↓ (File Descriptors)
[Connection Pool]
    ↓
[HTTP Parser] ← (Raw Data) ← [Request Buffer]
    ↓ (Parsed Request)
[Router]
    ↓
[Static File Server]  [CGI Executor]
    ↓                       ↓
[File System]      [External Process]
    ↓                       ↓
[Response Builder] ← (Data) ← [Output Parser]
    ↓ (Structured Data)
[HTTP Formatter]
    ↓ (Formatted Response)
[Response Buffer]
    ↓
[Socket Writer]
    ↓ (HTTP Response)
[İstemci]
```

### 4.2 Socket İşlem Akışı

```
Socket Oluşturma (socket())
        ↓
   Bind İşlemi (bind())
        ↓
   Listen İşlemi (listen())
        ↓
   Accept İşlemi (accept())
        ↓
Veri Okuma/Yazma (read()/write())
        ↓
Bağlantıyı Kapatma (close())
```

## 5. HTTP Metodları ve Durum Kodları

### 5.1 HTTP Metodları

| Metod  | Açıklama                          | İstemci Tarafından Gönderilen Veri |
|--------|-----------------------------------|-----------------------------------|
| GET    | Kaynak talep etme                 | URL parametreleri                 |
| POST   | Veri gönderme                     | Request body                      |
| PUT    | Kaynak oluşturma/güncelleme       | Request body                      |
| DELETE | Kaynak silme                      | Yok                               |
| HEAD   | Sadece header bilgisi talep etme  | Yok                               |

### 5.2 HTTP Durum Kodları

| Kod | Anlam                     | Açıklama                       |
|-----|---------------------------|--------------------------------|
| **1xx** | **Bilgilendirici**    | İşlem devam ediyor             |
| **2xx** | **Başarılı**          | İstek başarıyla tamamlandı     |
| 200 | OK                        | İstek başarılı                 |
| 201 | Created                   | Kaynak oluşturuldu             |
| 204 | No Content                | İçerik yok                     |
| **3xx** | **Yönlendirme**       | Ek işlem gerekiyor             |
| 301 | Moved Permanently         | Kalıcı yönlendirme             |
| 302 | Found                     | Geçici yönlendirme             |
| **4xx** | **İstemci Hatası**    | İstek hatalı                   |
| 400 | Bad Request               | Geçersiz istek                 |
| 403 | Forbidden                 | Yetkisiz erişim                |
| 404 | Not Found                 | Kaynak bulunamadı              |
| 405 | Method Not Allowed        | İzin verilmeyen metod          |
| **5xx** | **Sunucu Hatası**     | Sunucu hatası                  |
| 500 | Internal Server Error     | Sunucu iç hatası               |
| 501 | Not Implemented           | Metod implemente edilmemiş     |
| 503 | Service Unavailable       | Servis kullanılamıyor          |

## 6. Güvenlik Konuları

### 6.1 Temel Güvenlik Önlemleri

**Path Traversal Saldırıları:**

- `../../../etc/passwd` gibi istekleri engelleme
- Kök dizin (root) dışına çıkışı engelleme

**Buffer Overflow:**

- Sabit boyutlu buffer'lar yerine dinamik bellek yönetimi
- String işlemlerinde sınır kontrolü

**DoS (Denial of Service) Saldırıları:**

- Maksimum bağlantı sayısı sınırı
- Bağlantı zaman aşımı (timeout)
- Rate limiting

**CGI Güvenliği:**

- Environment variable temizleme
- Process izolasyonu
- Input sanitization

## 7. Performans Optimizasyonları

### 7.1 Caching Mekanizmaları

- **File Descriptor Cache:** Sık kullanılan dosyaların FD'lerini cache'leme
- **Content Cache:** Statik içerikleri memory'de tutma
- **Connection Pool:** TCP bağlantılarını yeniden kullanma

### 7.2 I/O Optimizasyonları

- **Non-blocking I/O:** Bloklanmadan çoklu bağlantı yönetimi
- **Zero-copy:** Veri kopyalamadan doğrudan ağa gönderme
- **Sendfile() Optimizasyonu:** Dosyaları kernel space'den doğrudan gönderme

## 8. Test Stratejileri

### 8.1 Test Türleri

- **Unit Tests:** Her bileşenin bağımsız testi
- **Integration Tests:** Bileşenlerin birlikte çalışması
- **Load Tests:** Yük altında performans testi
- **Security Tests:** Güvenlik açıklarının testi

### 8.2 Test Araçları

- **curl:** Manuel HTTP istekleri
- **ab (Apache Bench):** Yük testi
- **siege:** Stress testi
- **valgrind:** Memory leak kontrolü

## 9. Debugging Teknikleri

### 9.1 Debug Araçları

- **Logging:** Detaylı log kayıtları
- **GDB:** Debugger ile runtime analiz
- **strace:** Sistem çağrılarını izleme
- **tcpdump:** Ağ trafiğini analiz etme

### 9.2 Common Debug Senaryoları

**Socket Bağlantı Problemleri:**

- Port kullanımda mı?
- Firewall engelliyor mu?
- Bind izinleri var mı?

**HTTP Parsing Hataları:**

- CRLF (`\r\n`) eksik mi?
- Header formatı doğru mu?
- Chunked encoding destekleniyor mu?

**CGI Problemleri:**

- Process fork başarılı mı?
- Environment variables doğru mu?
- Execve çalışıyor mu?

## 10. Proje Yapısı Önerisi

```
webserver/
├── src/
│   ├── main.cpp              # Ana program
│   ├── WebServer.cpp         # Ana webserver sınıfı
│   ├── Socket.cpp            # Socket işlemleri
│   ├── HttpRequest.cpp       # HTTP istek parsing
│   ├── HttpResponse.cpp      # HTTP yanıt oluşturma
│   ├── ConfigParser.cpp      # Konfigürasyon parsing
│   ├── Router.cpp            # Routing mekanizması
│   ├── CgiHandler.cpp        # CGI işlemleri
│   └── utils/
│       ├── Logger.cpp        # Loglama
│       ├── StringUtils.cpp   # String işlemleri
│       └── FileUtils.cpp     # Dosya işlemleri
├── include/                  # Header dosyaları
├── config/                   # Konfigürasyon dosyaları
├── www/                      # Web içeriği
├── cgi-bin/                  # CGI programları
├── tests/                    # Test dosyaları
└── Makefile                  # Derleme dosyası
```

## 11. Kaynaklar ve Referanslar

### 11.1 RFC'ler (Request for Comments)

- **RFC 2616:** HTTP/1.1
- **RFC 3875:** CGI 1.1
- **RFC 793:** TCP Protocol

### 11.2 Kitaplar

- "Unix Network Programming" - W. Richard Stevens
- "HTTP: The Definitive Guide" - David Gourley
- "The Linux Programming Interface" - Michael Kerrisk

### 11.3 Online Kaynaklar

- Beej's Guide to Network Programming
- Mozilla Developer Network (MDN) HTTP Docs
- 42 Webserver Subject ve Ekleri

---

**Not:** Bu dokümantasyon, 42 Webserver projesinin teorik temellerini anlamak için hazırlanmıştır. Her katman, belirli bir sorumluluğu yerine getirir ve bir sonraki katmana soyutlanmış bir arayüz sunar. Bu, yazılım mühendisliğindeki "separation of concerns" (ilgi alanlarının ayrılması) prensibinin uygulanmasıdır.