import tkinter as tk
from tkinter import filedialog, messagebox
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
from matplotlib.figure import Figure
import pandas as pd
import numpy as np

class GUI():
    #inicjalizacja głównej klasy
    def __init__(self, size):
        self.Window = tk.Tk()
        self.Window.title("Stanowisko laboratoryjne do badania wydajności paneli solarnych")
        self.Window.geometry(size)
        self.Window.resizable(0, 0)

        self.data = None
        self.canvas = None
        self.toolbar = None
        self.current_figure = None

        #tło w aplikacji
        background = tk.PhotoImage(file="images.png")
        label1 = tk.Label(self.Window, image=background)
        label1.place(x=0, y=0)

        #przyciski w głównym oknie
        button1 = tk.Button(self.Window, text="Wykresy", width=20, height=2, font=("Arial", 14), command=self.Figures)
        button1.pack(pady=20)
        button2 = tk.Button(self.Window, text="Wczytaj dane z karty SD", width=20, height=2, font=("Arial", 14), command=self.read_sd_file)
        button2.pack(pady=20)
        button3 = tk.Button(self.Window, text="Autorzy", width=20, height=2, font=("Arial", 14), command=self.authors_info)
        button3.pack(pady=20)
        button4 = tk.Button(self.Window, text="Wyjście", width=20, height=2, font=("Arial", 14), command=self.close_app)
        button4.pack(pady=20)

        self.Window.mainloop()

    #funkcja obsługująca okno wczytania pliku CSV
    def read_sd_file(self):
        SDwindow = tk.Toplevel()
        SDwindow.title("Wczytaj dane z karty SD")
        SDwindow.geometry("600x400")
        SDwindow.resizable(0, 0)

        background = tk.PhotoImage(file="images.png")
        label_bg = tk.Label(SDwindow, image=background)
        label_bg.place(x=0, y=0, relwidth=1, relheight=1)

        # funckja wczytująca plik CSV
        def load_file():
            file_path = filedialog.askopenfilename(
                filetypes=[("CSV files", "*.csv"), ("Wszystkie pliki", "*.*")],
                title="Wybierz plik CSV"
            )
            if not file_path:
                return

            try:
                #Wczytanie danych z pliku CSV
                self.data = pd.read_csv(file_path, delimiter=";", decimal=",", dtype={"Data": str, "Godzina": str},encoding="windows-1250")

                #Rozpoznianie formatu daty
                try:
                    self.data["Data"] = pd.to_datetime(self.data["Data"], format='%d.%m.%Y')
                except ValueError:
                    try:
                        self.data["Data"] = pd.to_datetime(self.data["Data"], format='%m/%d/%Y')
                    except ValueError:
                        messagebox.showerror("Błąd", "Nie rozpoznano formatu daty. Sprawdź dane w pliku CSV.")
                        self.data = None
                        return

                #Przetwarzanie czasu
                self.data["Godzina"] = pd.to_datetime(self.data["Godzina"], format='%H:%M:%S').dt.time

                messagebox.showinfo("Informacja", "Plik CSV został wczytany.")
            except Exception as e:
                messagebox.showerror("Błąd", f"Nie udało się wczytać pliku: {e}")
                self.data = None

                messagebox.showinfo("Informacja", "Plik CSV został wczytany.")
            except Exception as e:
                messagebox.showerror("Błąd", f"Nie udało się wczytać pliku: {e}")
                self.data = None

        #przyciski odpowiedzialne za wczytanie pliku CSV
        load_button = tk.Button(SDwindow, text="Wybierz plik CSV", font=("Arial", 14), command=load_file)
        load_button.pack(pady=20)
        close_button = tk.Button(SDwindow, text="Powrót", font=("Arial", 14), command=SDwindow.destroy)
        close_button.pack(pady=20)

        SDwindow.mainloop()

    #funkcja odpowiedzialna za wykresy
    def Figures(self):
        if not hasattr(self, 'data') or self.data is None:
            messagebox.showerror("Błąd", "Najpierw wczytaj dane z pliku CSV!")
            return

        #podstawowa konfiguracja
        Figures = tk.Toplevel()
        Figures.title("Wykresy")
        Figures.geometry("1400x800")
        Figures.resizable(1, 1)

        figure_frame = tk.Frame(Figures)
        figure_frame.grid(row=0, column=0, sticky="nsew")

        controls_frame = tk.Frame(Figures)
        controls_frame.grid(row=1, column=0, sticky="ew")

        statistics_frame = tk.Frame(Figures)
        statistics_frame.grid(row=0,column=1,sticky="ns")

        Figures.rowconfigure(0, weight=1)
        Figures.columnconfigure(0, weight=1)

        self.canvas = None
        self.toolbar = None

        #konfiguracja wyboru osi X oraz Y
        x_column_variable = tk.StringVar(Figures)

        columns = list(self.data.columns)
        x_column_variable.set(columns[0])

        tk.Label(controls_frame, text="Oś X:", font=("Arial", 12)).grid(row=0, column=0, padx=5, pady=5)
        x_dropdown = tk.OptionMenu(controls_frame, x_column_variable, *columns)
        x_dropdown.grid(row=0, column=1, padx=5, pady=5)

        tk.Label(controls_frame, text="Oś Y:", font=("Arial", 12)).grid(row=0, column=2, padx=5, pady=5)
        y_listbox = tk.Listbox(controls_frame, selectmode=tk.MULTIPLE, exportselection=0, height=5)
        for col in columns:
            y_listbox.insert(tk.END, col)
        y_listbox.grid(row=0, column=3, padx=5, pady=5)

        #możliwość wyboru zakresu danych
        tk.Label(controls_frame, text="Początkowy indeks:", font=("Arial", 12)).grid(row=1, column=0, padx=5, pady=5)
        start_index_entry = tk.Entry(controls_frame, width=10)
        start_index_entry.grid(row=1, column=1, padx=5, pady=5)
        start_index_entry.insert(0, "0")

        tk.Label(controls_frame, text="Ilość punktów:", font=("Arial", 12)).grid(row=1, column=2, padx=5, pady=5)
        num_points_entry = tk.Entry(controls_frame, width=10)
        num_points_entry.grid(row=1, column=3, padx=5, pady=5)
        num_points_entry.insert(0, str(len(self.data)))

        #okno statystyk
        statistics_label = tk.Label(statistics_frame, text="Statystka", font=("Arial",14,"bold"),bg="#f0f0f0")
        statistics_label.pack(pady=10)

        statistics_text = tk.Text(statistics_frame, width=40,height=40,wrap=tk.WORD, font=("Arial",12))
        statistics_text.pack(pady=10, fill=tk.BOTH, expand=True)

        #funkcja wyświetlająca wykresy
        def show_plot():
            try:
                #Pobranie zakresu danych
                start_index = int(start_index_entry.get())
                num_points = int(num_points_entry.get())

                if start_index < 0 or num_points <= 0:
                    raise ValueError("Indeks początkowy i ilość punktów muszą być liczbami dodatnimi.")

                #Sprawdzenie czy zakres sie mieści
                end_index = min(start_index + num_points, len(self.data))

                x_column = x_column_variable.get()
                y_indices = y_listbox.curselection()  #Pobranie zaznaczonych indeksów dla osi Y
                y_columns = [columns[i] for i in y_indices]

                if x_column not in self.data.columns or not y_columns:
                    messagebox.showerror("Błąd", "Nieprawidłowe kolumny wybrane.")
                    return

                x = self.data[x_column].iloc[start_index:end_index]

                if x_column == "Data":
                    x = pd.to_datetime(x, format='%d.%m.%Y')

                elif x_column == "Godzina":
                    x = self.data["Godzina"].iloc[start_index:end_index].apply(
                        lambda t: t.hour * 3600 + t.minute * 60 + t.second
                    )
                    time_ticks = self.data["Godzina"].iloc[start_index:end_index]

                if self.canvas:
                    self.canvas.get_tk_widget().destroy()
                if self.toolbar:
                    self.toolbar.destroy()

                fig = Figure(figsize=(5, 4), dpi=100)
                plot = fig.add_subplot(111)

                statistics_text.delete(1.0, tk.END)  #Wyczyść okno statystyk

                #Rysowanie wielu serii danych i obliczanie statystyk
                for y_column in y_columns:
                    y = self.data[y_column].iloc[start_index:end_index]
                    plot.plot(x, y, marker='o',linestyle="None",label=f"{y_column}({x_column})")


                    trend_var = np.polyfit(x,y,1)
                    trendline= np.polyval(trend_var,x)
                    plot.plot(x,trendline,linestyle='--',label=f"Linia trendu {y_column}")

                    #Obliczanie statystyk
                    mean_val = y.mean()
                    median_val = y.median()
                    std_val = y.std()
                    min_val = y.min()
                    max_val = y.max()

                    #Wyświetlanie statystyk
                    statistics_text.insert(tk.END, f"Statystyki dla {y_column}:\n")
                    statistics_text.insert(tk.END, f"  Średnia: {mean_val:.2f}\n")
                    statistics_text.insert(tk.END, f"  Mediana: {median_val:.2f}\n")
                    statistics_text.insert(tk.END, f"  Odchylenie standardowe: {std_val:.2f}\n")
                    statistics_text.insert(tk.END, f"  Minimum: {min_val:.2f}\n")
                    statistics_text.insert(tk.END, f"  Maksimum: {max_val:.2f}\n")
                    statistics_text.insert(tk.END, "\n")

                plot.set_title(f",".join([f"{y_col}({x_column})" for y_col in y_columns]))
                plot.set_xlabel(x_column)
                plot.set_ylabel(", ".join(y_columns))
                plot.grid()
                plot.legend()

                if x_column == "Godzina":
                    plot.set_xticks(x)
                    plot.set_xticklabels(time_ticks, rotation=45, ha="right",fontsize=8)

                self.canvas = FigureCanvasTkAgg(fig, master=figure_frame)
                self.canvas.draw()
                self.canvas.get_tk_widget().pack(expand=True, fill="both")

                self.toolbar = NavigationToolbar2Tk(self.canvas, figure_frame)
                self.toolbar.update()
                self.toolbar.pack(side=tk.BOTTOM, fill=tk.X)

            except ValueError as e:
                messagebox.showerror("Błąd", f"Błąd w zakresie danych: {e}")

        #funkjca zapisująca wykresy
        def save_plot():
            if not self.canvas:
                messagebox.showerror("Błąd", "Najpierw wygeneruj wykres, aby zapisać!")
                return
            file_path = filedialog.asksaveasfilename(
                defaultextension=".png",
                filetypes=[("Pliki PNG", "*.png"), ("Wszystkie pliki", "*.*")]
            )
            if not file_path:
                return
            try:
                self.canvas.figure.savefig(file_path)
                messagebox.showinfo("Sukces", f"Wykres zapisano jako {file_path}")
            except Exception as e:
                messagebox.showerror("Błąd", f"Nie udało się zapisać wykresu: {e}")

        #przyciski odpowiedzialna za zapis
        wykres_button = tk.Button(
            controls_frame,
            text="Wykres",
            font=("Arial", 14),
            command=show_plot,
        )
        wykres_button.grid(row=0, column=4, padx=10, pady=10)

        save_button = tk.Button(
            controls_frame,
            text="Zapisz Wykres",
            font=("Arial", 14),
            command=save_plot,
        )
        save_button.grid(row=0, column=5, padx=10, pady=10)

        close_button = tk.Button(
            controls_frame,
            text="Zamknij",
            font=("Arial", 14),
            command=Figures.destroy,
            bg="#f0f0f0"
        )
        close_button.grid(row=0, column=6, padx=10, pady=10)

    #okno autorów
    def authors_info(self):
        Authors = tk.Toplevel()
        Authors.title("Autorzy")
        Authors.geometry("600x400")
        Authors.resizable(0, 0)
        background = tk.PhotoImage(file="images.png")
        label_bg = tk.Label(Authors, image=background)
        label_bg.place(x=0, y=0, relwidth=1, relheight=1)
        tk.Label(
            Authors,
            text="Aplikacja stworzona przez:\n- Jakub Stankiewicz 151140\n- Michał Stacherski 151087\n- Bartosz Skowroński 151117",
            font=("Arial", 16),
            bg="#ffffff",
            fg="#000000"
        ).pack(pady=20)
        tk.Button(Authors, text="Zamknij", font=("Arial", 14), command=Authors.destroy, bg="#f0f0f0").pack(pady=10)

    def close_app(self):
        self.Window.destroy()

#wymiary okna startowego
SIZE = '600x400'

if __name__ == '__main__':
    window = GUI(SIZE)
