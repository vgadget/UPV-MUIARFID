from http.server import HTTPServer, BaseHTTPRequestHandler
from io import BytesIO
from urllib import response
from urllib.parse import urlparse
from Traffic_Sign_Recognition import predictClass

class SimpleHTTPRequestHandler(BaseHTTPRequestHandler):

    def do_GET(self):
        self.send_response(200)
        self.end_headers()
        response = 'Predicted class: ' + str(predictClass(urlparse(self.path)))
        self.wfile.write(str.encode(response))
        
    def do_POST(self):
        content_length = int(self.headers['Content-Length'])
        body = self.rfile.read(content_length)
        self.send_response(200)
        self.end_headers()
        (predicted_class, final_img) = predictClass(body)
        response_text = 'Predicted class: ' + str(predicted_class)
        response = BytesIO()
        response.write(str.encode(response_text))
        self.wfile.write(response.getvalue())


httpd = HTTPServer(('0.0.0.0', 8080), SimpleHTTPRequestHandler)
httpd.serve_forever()