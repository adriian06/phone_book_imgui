#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <GLFW/glfw3.h>
#include <vector>
#include <string>
#include <sstream>

typedef struct phone_record {
    char* name;
    char* phone_number;
}phone_record;

typedef struct phone_book{
    int size;
    int record_count;
    phone_record* records;
}phone_book;

phone_book init(){
    phone_book book = {0,0,NULL};
    return book;
}

void add_record(phone_book* self, phone_record record){

    if(self->size == 0){
        self->size = 1;
        self->records = (phone_record*)malloc(sizeof (phone_record));
        self->records[0] = record;
        self->record_count = 1;
        return;
    }
    if(self->size == self->record_count){
        self->size = self->size * 2;
        self->records = (phone_record*)realloc(self->records,self->size * sizeof(phone_record));
        self->records[self->record_count]=record;
        self->record_count++;
        return;
    }
    self->records[self->record_count]=record;
    self->record_count++;
}

void delete_record(phone_book* self, int index){
    if(index < 0 || index >= self->record_count){
        return;
    }
    free(self->records[index].name);
    free(self->records[index].phone_number);
    for(int i=index;i<self->record_count-1;i++){
        self->records[i] = self->records[i+1];
    }
    self->record_count--;
}

void print_record_size(phone_record record){
    long long int char_size = sizeof (char);
    long long int charp_size = sizeof (char*);
    size_t name_len = strlen(record.name);
    size_t phone_len = strlen(record.phone_number);
    int final_size = name_len * char_size + phone_len * char_size + 2 * charp_size;
    printf("%d(size) = 2 *%llu (sizeof char*) + %zu (len(\"%s\")) * %llu (sizeof char) + %zu (len(\"%s\")) * %llu (sizeof char)\n",
        final_size,
        charp_size,
        name_len,
        record.name,
        char_size,
        phone_len,
        record.phone_number,
        char_size
    );
}

void print_book(phone_book self){
    int allocated_size = self.size * sizeof (phone_record);
    printf("allocated_size: %i = %i * %llu (sizeof phone_record)\n",allocated_size,self.size,sizeof (phone_record));

    int used_size =self.record_count * sizeof (phone_record);
    printf("used: %i = %i * %llu (sizeof phone_record)\n",used_size,self.record_count,sizeof (phone_record));

    printf("start address: %p\n",self.records);
    printf("end address: %p\n",self.records + self.size);
    for(int i=0;i<self.record_count;i++){
        printf("item %i starts at address %p ends at %p name: %s phone: %s\n",
        i+1,
        self.records + i,
        self.records + i + 1,
        self.records[i].name,
        self.records[i].phone_number);
        print_record_size(self.records[i]);
        }
}

int calculate_book_size(phone_book self){
    int allocated_size = self.size * sizeof (phone_record);
    for(int i = 0; i < self.record_count; i++){
        allocated_size += strlen(self.records[i].name) + 1;
        allocated_size += strlen(self.records[i].phone_number) + 1;
    }
    return allocated_size;
}

void destroy(phone_book* self){
    if(self->records != NULL) {
        free(self->records);
        self->records = NULL;
    }
    self->size = 0;
    self->record_count = 0;
}

int main(int, char**)
{
    // glfw init
    if (!glfwInit())
        return -1;
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Telephone Agenda", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // vsync

    // context imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

    char name[128] = "";
    char number[128] = "";
    phone_book book = init();

    while (!glfwWindowShouldClose(window))
    {

        glfwPollEvents();

        // Start the ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Telephone Agenda");

        ImGui::InputText("Name", name, IM_ARRAYSIZE(name));
        ImGui::InputText("Number", number, IM_ARRAYSIZE(number));
        if (ImGui::Button("Add Contact")) {
            char* name_buffer = (char*)malloc( 128);
            char* number_buffer = (char*)malloc(128);
            strcpy(name_buffer,name);
            strcpy(number_buffer,number);
            phone_record record = {name_buffer,number_buffer};
            add_record(&book,record);
            name[0]   = '\0';
            number[0] = '\0';
        }

        ImGui::Separator();

        ImGui::Text("Contacts:");
        for(int i=0;i<book.record_count;i++){
            std::stringstream ss;
            ss << book.records[i].name << " - " << book.records[i].phone_number;
            std::string str = ss.str();
            ImGui::Text(str.c_str());
            ImGui::SameLine();
            std::string button_id = "Delete##" + std::to_string(i);
            if(ImGui::Button(button_id.c_str())){
                delete_record(&book,i);
            }
        }

        ImGui::Separator();

        ImGui::Text("Book size: %i bytes",calculate_book_size(book));

        ImGui::Separator();

        if(ImGui::Button("Print Advanced phone book details")){
            print_book(book);
        }

        ImGui::Separator();

        if(ImGui::Button("Destroy phone book")){
            destroy(&book);
        }

        ImGui::End();

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

